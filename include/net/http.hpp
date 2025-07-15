/**
 * @brief HTTP服务器，提供HTTP/1.1协议支持
 * @author Ricky
 * @date 2025/7/14
 * @version 1.0
 */
#ifndef HTTP_HPP
#define HTTP_HPP

#include "Dict.hpp"
#include "StringBuilder.hpp"
#include "ThreadPool.hpp"
#include "tcp.hpp"
#include "Log.hpp"

namespace my::net {

#undef DELETE // 避免与HTTP方法冲突

/**
 * @brief HTTP方法
 */
enum class HttpMethod {
    GET,
    POST,
    PUT,
    DELETE,
    CONNECT,
    OPTIONS,
    TRACE,
    PATCH,
    UNKNOWN
};

/**
 * @brief HTTP状态码
 */
enum class HttpStatusCode {
    OK = 200,
    CREATED = 201,
    ACCEPTED = 202,
    NO_CONTENT = 204,
    MOVED_PERMANENTLY = 301,
    FOUND = 302,
    NOT_MODIFIED = 304,
    BAD_REQUEST = 400,
    UNAUTHORIZED = 401,
    FORBIDDEN = 403,
    NOT_FOUND = 404,
    METHOD_NOT_ALLOWED = 405,
    REQUEST_TIMEOUT = 408,
    PAYLOAD_TOO_LARGE = 413,
    INTERNAL_SERVER_ERROR = 500,
    NOT_IMPLEMENTED = 501,
    BAD_GATEWAY = 502,
    SERVICE_UNAVAILABLE = 503,
};

/**
 * @brief HTTP状态码与文本的映射
 */
static const util::Dict<HttpStatusCode, CString> status_text_map = {
    {HttpStatusCode::OK, "OK"},
    {HttpStatusCode::CREATED, "Created"},
    {HttpStatusCode::ACCEPTED, "Accepted"},
    {HttpStatusCode::NO_CONTENT, "No Content"},
    {HttpStatusCode::MOVED_PERMANENTLY, "Moved Permanently"},
    {HttpStatusCode::FOUND, "Found"},
    {HttpStatusCode::NOT_MODIFIED, "Not Modified"},
    {HttpStatusCode::BAD_REQUEST, "Bad Request"},
    {HttpStatusCode::UNAUTHORIZED, "Unauthorized"},
    {HttpStatusCode::FORBIDDEN, "Forbidden"},
    {HttpStatusCode::NOT_FOUND, "Not Found"},
    {HttpStatusCode::METHOD_NOT_ALLOWED, "Method Not Allowed"},
    {HttpStatusCode::REQUEST_TIMEOUT, "Request Timeout"},
    {HttpStatusCode::PAYLOAD_TOO_LARGE, "Payload Too Large"},
    {HttpStatusCode::INTERNAL_SERVER_ERROR, "Internal Server Error"},
    {HttpStatusCode::NOT_IMPLEMENTED, "Not Implemented"},
    {HttpStatusCode::BAD_GATEWAY, "Bad Gateway"},
    {HttpStatusCode::SERVICE_UNAVAILABLE, "Service Unavailable"},
};

/**
 * @brief HTTP请求
 */
class HttpRequest : public Object<HttpRequest> {
public:
    using Self = HttpRequest;

    /**
     * @brief 从请求头获取 Content-Length
     */
    usize content_length() const {
        auto content_length = headers_.get_or_default("Content-Length", "0");
        return std::stoul(content_length.into_string());
    }

private:
    HttpMethod method_ = HttpMethod::UNKNOWN;        // 请求方法
    CString path_;                                   // 请求路径
    CString version_;                                // HTTP版本
    util::Dict<CString, util::String> headers_;      // 请求头
    util::String body_;                              // 请求体
    util::Dict<CString, util::String> query_params_; // 查询参数
};

/**
 * @brief HTTP响应
 */
class HttpResponse : public Object<HttpResponse> {
public:
    using Self = HttpResponse;

    /**
     * @brief 获取状态文本
     */
    CString status_text() const {
        return status_text_map.get_or_default(status_, "Unknown");
    }

    /**
     * @brief 获取响应状态码
     */
    HttpStatusCode get_status() const {
        return status_;
    }

    /**
     * @brief 设置响应状态码
     */
    void set_status(HttpStatusCode status) {
        status_ = status;
    }

    /**
     * @brief 获取响应头
     */
    util::Dict<CString, util::String> get_headers() const {
        return headers_;
    }

    /**
     * @brief 设置响应头中的 Content-Type
     */
    void set_content_type(const util::String& type) {
        headers_["Content-Type"] = type;
    }

    /**
     * @brief 获取响应体
     */
    util::String get_body() const {
        return body_;
    }

    /**
     * @brief 设置响应体
     */
    void set_body(const util::String& content, const util::String& type = "text/plain") {
        body_ = content;
        set_content_type(type);
        headers_["Content-Length"] = util::String::from_u64(body_.length());
    }

    /**
     * @brief 设置重定向
     */
    void set_redirect(const util::String& location, HttpStatusCode code = HttpStatusCode::FOUND) {
        status_ = code;
        headers_["Location"] = location;
        body_.clear();
        headers_.remove("Content-Length");
    }

private:
    HttpStatusCode status_ = HttpStatusCode::OK; // 响应状态
    util::Dict<CString, util::String> headers_;  // 响应头
    util::String body_;                          // 响应体
};

/**
 * @brief 路由处理函数类型
 */
using RouterHandler = std::function<HttpResponse(const HttpRequest&)>;

/**
 * @class HttpServer
 * @brief HTTP服务器
 * @note 该类是线程安全的
 */
class HttpServer : public Object<HttpServer> {
public:
    using Self = HttpServer;

    /**
     * @brief 构造函数
     * @param ip 监听IP地址
     * @param port 监听端口
     * @param num_of_threads 线程数
     * @param max_connections 最大并发连接数，0表示无限制
     */
    HttpServer(const char* ip, u16 port, usize num_of_threads = 4, u32 max_connections = 0) :
            server_(ip, port), pool_(num_of_threads), max_connections_(max_connections) {}

    /**
     * @brief 启动HTTP服务器
     */

    /**
     * @brief 启动HTTP服务器
     * @note 该函数会阻塞直到服务器关闭
     */
    void start() {
        io::Log::info("HTTP server started on {}:{}", SRC_LOC, server_.get_local_ip(), server_.get_local_port());

        while (true) {
            try {
                // 接收新连接
                auto& client = server_.accept();

                // 检查连接限制
                if (max_connections_ > 0 && active_connections_ >= max_connections_) {
                    io::Log::info("Connection limit reached ({}/{})", SRC_LOC, active_connections_, max_connections_);
                    send_error_response(client, HttpStatusCode::SERVICE_UNAVAILABLE);
                    client.close();
                    continue;
                }

                // 在新线程中处理请求
                auto future = pool_.push([this, &client]() {
                    ++active_connections_;
                    handle_connection(client);
                    client.close();
                    --active_connections_;
                });
                future.get();
            } catch (const std::exception& ex) {
                io::Log::info("Connection error: {}", SRC_LOC, ex.what());
            }
        }
    }

    /**
     * @brief 添加路由处理函数
     * @param method HTTP方法
     * @param path 请求路径
     * @param handler 处理函数
     */
    void add_route(HttpMethod method, const util::String& path, RouterHandler handler) {
        routes_[method][path] = handler;
    }

    /**
     * @brief 添加GET路由
     * @param path 请求路径
     * @param handler 处理函数
     */
    void get(const util::String& path, RouterHandler handler) {
        add_route(HttpMethod::GET, path, handler);
    }

    /**
     * @brief 添加POST路由
     * @param path 请求路径
     * @param handler 处理函数
     */
    void post(const util::String& path, RouterHandler handler) {
        add_route(HttpMethod::POST, path, handler);
    }

    /**
     * @brief 添加静态文件服务
     * @param url_path URL路径前缀
     * @param fs_path 文件系统路径
     * @param cache_max_age 缓存最大时间（秒），0表示不缓存
     */
    void static_file(const util::String& url_path, const util::String fs_path, u32 cache_max_age = 0) {
        static_dirs_[url_path] = {fs_path, cache_max_age};
    }

private:
    /**
     * @brief 静态路径配置
     */
    struct StaticDirConfig {
        util::String fspath;   // 文件系统路径
        u32 cache_max_age = 0; // 缓存时间（秒）
    };

    /**
     * @brief 发送错误响应
     * @note 响应体格式
     * <html>
     * <head>
     *   <title>
     *     `${status} ${status_text}`
     *   </title>
     * </head>
     * <body>
     *   <h1>
     *     `${status} ${status_text}`
     *   </h1>
     * </body>
     * </html>
     */
    void send_error_response(Socket& client, HttpStatusCode status) {
        HttpResponse resp;
        resp.set_status(status);

        util::StringBuilder sb;
        sb.append("<html><head><title>")
            .append(util::String::from_u32(static_cast<i32>(status)))
            .append(" ")
            .append(status_text_map.get(status))
            .append("</title></head><body><h1>")
            .append(util::String::from_u32(static_cast<i32>(status)))
            .append(" ")
            .append(status_text_map.get(status))
            .append("</h1></body></html>");

        resp.set_body(sb.build(), "text/html");
        send_response(client, resp);
    }

    /**
     * @brief 发送响应
     * @param client 客户端
     * @param resp HTTP响应
     */
    void send_response(Socket& client, const HttpResponse& resp) {
        std::ostringstream oss;

        // 状态行
        oss << "HTTP/1.1 " << static_cast<i32>(resp.get_status())
            << " " << resp.status_text().data() << "\r\n";

        // 头部
        for (const auto& [key, value] : resp.get_headers()) {
            oss << key << ": " << value << "\r\n";
        }

        // 空行分隔头部和主体
        oss << "\r\n";

        // 主体
        auto body = resp.get_body();
        if (!body.empty()) {
            oss << body;
        }

        // 发送响应
        auto resp_str = oss.str();
        client.send(resp_str.c_str(), resp_str.size());
    }

    /**
     * @brief 处理客户端连接
     * @param client 客户端
     */
    void handle_connection(Socket& client) {
        // TODO
    }

private:
    TcpServer server_;                                                       // TCP服务器
    async::ThreadPool pool_;                                                 // 线程池
    util::Dict<HttpMethod, util::Dict<util::String, RouterHandler>> routes_; // 路由表
    util::Dict<util::String, StaticDirConfig> static_dirs_;                  // 静态路径表 TODO 命名
    std::atomic<u32> active_connections_{0};                                 // 当前活跃连接数
    u32 max_connections_{0};                                                 // 最大连接数
};

} // namespace my::net

#endif // HTTP_HPP