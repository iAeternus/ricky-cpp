/**
 * @brief HTTP服务器，提供HTTP/1.1协议支持
 * @author Ricky
 * @date 2025/7/20
 * @version 1.1
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
    HEAD,
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
    HTTP_VERSION_NOT_SUPPORTED = 505,
};

/**
 * @brief HTTP状态码与文本的映射
 */
static const util::Dict<HttpStatusCode, util::String> status_text_map = {
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
    {HttpStatusCode::HTTP_VERSION_NOT_SUPPORTED, "HTTP Version Not Supported"},
};

/**
 * @brief HTTP请求
 */
struct HttpRequest : public Object<HttpRequest> {
    using Self = HttpRequest;

    HttpMethod method = HttpMethod::UNKNOWN;             // 请求方法
    util::String path;                                   // 请求路径
    util::String version;                                // HTTP版本
    util::Dict<util::String, util::String> headers;      // 请求头
    util::String body;                                   // 请求体
    util::Dict<util::String, util::String> query_params; // 查询参数

    /**
     * @brief 从请求头获取 Content-Length
     */
    usize content_length() const {
        auto content_length = headers.get_or_default("Content-Length"_s, "0");
        return std::stoul(content_length.into_string());
    }

    /**
     * @brief 获取HTTP版本号
     */
    float http_version() const {
        if (version.starts_with("HTTP/1.1"_s)) return 1.1f;
        if (version.starts_with("HTTP/1.0"_s)) return 1.0f;
        if (version.starts_with("HTTP/2.0"_s)) return 2.0f;
        return 0.0f;
    }
};

/**
 * @brief HTTP响应
 */
struct HttpResponse : public Object<HttpResponse> {
    using Self = HttpResponse;

    HttpStatusCode status = HttpStatusCode::OK;     // 响应状态
    util::Dict<util::String, util::String> headers; // 响应头
    util::String body;                              // 响应体

    /**
     * @brief 获取状态文本
     */
    util::String status_text() const {
        return status_text_map.get_or_default(status, "Unknown");
    }

    /**
     * @brief 设置响应头中的 Content-Type
     */
    void set_content_type(const util::String& type) {
        headers["Content-Type"_s] = type;
    }

    /**
     * @brief 设置响应体
     */
    void set_body(const util::String& content, const util::String& type = "text/plain") {
        body = content;
        set_content_type(type);
        headers["Content-Length"_s] = util::String::from_u64(body.length());
    }

    /**
     * @brief 设置重定向 TODO 有问题！
     */
    void set_redirect(const util::String& location, const HttpStatusCode code = HttpStatusCode::FOUND) {
        status = code;
        headers["Location"_s] = location;
        body.clear();
        headers.remove("Content-Length");

        util::StringBuilder sb;
        sb.append("<html><head><title>Redirect</title></head>")
            .append("<body><h1>Redirect</h1><p>You are being redirected to <a href=\"")
            .append(location)
            .append("\">")
            .append(location)
            .append("</a></p></body></html>");
        set_body(sb.build(), "text/html");
    }
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
     * @brief 静态路径配置
     */
    struct StaticDirConfig {
        util::String fspath;   // 文件系统路径
        u32 cache_max_age = 0; // 缓存时间（秒）
    };

    /**
     * @brief 构造函数
     * @param ip 监听IP地址
     * @param port 监听端口
     * @param num_of_threads 线程数
     * @param max_connections 最大并发连接数，0表示无限制
     */
    HttpServer(const char* ip, u16 port, usize num_of_threads = 4, u32 max_connections = 0, u32 timeout = 30) :
            server_(ip, port), pool_(num_of_threads), max_connections_(max_connections), timeout_(timeout) {
        // 添加默认MIME类型
        mime_types_["html"_s] = "text/html";
        mime_types_["css"_s] = "text/css";
        mime_types_["js"_s] = "application/javascript";
        mime_types_["json"_s] = "application/json";
        mime_types_["png"_s] = "image/png";
        mime_types_["jpg"_s] = "image/jpeg";
        mime_types_["jpeg"_s] = "image/jpeg";
        mime_types_["gif"_s] = "image/gif";
        mime_types_["svg"_s] = "image/svg+xml";
        mime_types_["ico"_s] = "image/x-icon";
        mime_types_["txt"_s] = "text/plain";
    }

    /**
     * @brief 启动HTTP服务器
     * @note 该函数会阻塞直到服务器关闭
     */
    void start() {
        io::Log::info("HTTP server started on {}:{}", SRC_LOC, server_.get_local_ip(), server_.get_local_port());

        loop {
            try {
                // 接受新连接
                auto client = server_.accept();
                if (!client || !client->is_valid()) continue;

                // 设置超时
                client->set_timeout(timeout_ * 1000, true);  // 接收超时
                client->set_timeout(timeout_ * 1000, false); // 发送超时

                // 检查连接限制
                if (max_connections_ > 0 && active_connections_.load() >= max_connections_) {
                    io::Log::warn("Connection limit reached ({}/{})", SRC_LOC, active_connections_.load(), max_connections_);
                    send_error_response(*client, HttpStatusCode::SERVICE_UNAVAILABLE);
                    client->close();
                    continue;
                }

                // 在新线程中处理请求
                pool_.push([this, client]() {
                    ++active_connections_;
                    try {
                        handle_connection(*client);
                    } catch (const std::exception& ex) {
                        io::Log::error("Request processing error: {}", SRC_LOC, ex.what());
                    }
                    client->close();
                    --active_connections_;
                });
            } catch (const std::exception& ex) {
                io::Log::error("Connection error: {}", SRC_LOC, ex.what());
            }
        }
    }

    /**
     * @brief 添加路由处理函数
     * @param method HTTP方法
     * @param path 请求路径
     * @param handler 处理函数
     */
    void add_route(HttpMethod method, const util::String& path, const RouterHandler& handler) {
        std::lock_guard<std::mutex> lock(routes_mutex_);
        routes_[method][path] = handler;
    }

    /**
     * @brief 添加GET路由
     * @param path 请求路径
     * @param handler 处理函数
     */
    void get(const util::String& path, const RouterHandler& handler) {
        add_route(HttpMethod::GET, path, handler);
    }

    /**
     * @brief 添加POST路由
     * @param path 请求路径
     * @param handler 处理函数
     */
    void post(const util::String& path, const RouterHandler& handler) {
        add_route(HttpMethod::POST, path, handler);
    }

    /**
     * @brief 添加静态文件服务
     * @param url_path URL路径前缀
     * @param fs_path 文件系统路径
     * @param cache_max_age 缓存最大时间（秒），0表示不缓存
     */
    void static_file(const util::String& url_path, const util::String& fs_path, u32 cache_max_age = 0) {
        static_dirs_[url_path] = {fs_path, cache_max_age};
    }

    /**
     * @brief 添加自定义MIME类型
     * @param extension 文件扩展名
     * @param mime_type MIME类型
     */
    void add_mime_type(const util::String& extension, const util::String& mime_type) {
        mime_types_[extension] = mime_type;
    }

private:
    /**
     * @brief 发送错误响应
     */
    static void send_error_response(const Socket& client, HttpStatusCode status) {
        HttpResponse resp;
        resp.status = status;

        util::StringBuilder sb;
        sb.append("<html><head><title>")
            .append(util::String::from_u32(static_cast<u32>(status)))
            .append(" ")
            .append(status_text_map.get(status))
            .append("</title></head><body><h1>")
            .append(util::String::from_u32(static_cast<u32>(status)))
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
    static void send_response(const Socket& client, const HttpResponse& resp) {
        util::StringBuilder response_builder;

        // 状态行
        response_builder.append("HTTP/1.1 ")
            .append(util::String::from_u32(static_cast<u32>(resp.status)))
            .append(" ")
            .append(resp.status_text())
            .append("\r\n");

        // 头部
        for (const auto& [key, value] : resp.headers) {
            response_builder.append(key)
                .append(": ")
                .append(value)
                .append("\r\n");
        }

        // 空行
        response_builder.append("\r\n");

        // 主体
        if (!resp.body.empty()) {
            response_builder.append(resp.body);
        }

        // 发送响应
        auto response_str = response_builder.build();
        io::Log::debug("Sending response: {}", SRC_LOC, response_str);
        client.send_bytes(response_str.__str__().data(), response_str.length());
    }

    /**
     * @brief 处理客户端连接
     * @param client 客户端
     */
    void handle_connection(Socket& client) {
        try {
            // 接收并解析HTTP请求
            auto req = parse_request(client);
            io::Log::info("Request: {} {}", SRC_LOC, req.method, req.path);

            // 检查HTTP版本
            if (req.http_version() > 1.1f) {
                throw runtime_exception("Unsupported HTTP version: {}", req.version);
            }

            // 处理静态文件请求
            if (handle_static_file(req, client)) {
                return;
            }

            HttpResponse resp;
            {
                std::lock_guard<std::mutex> lock(routes_mutex_);

                // 检查方法是否存在
                if (routes_.contains(req.method)) {
                    auto& method_routes = routes_[req.method];

                    // 检查路径是否存在
                    if (method_routes.contains(req.path)) {
                        resp = method_routes[req.path](req);
                    } else {
                        resp.status = HttpStatusCode::NOT_FOUND;
                        util::StringBuilder sb;
                        sb.append("<h1>404 Not Found</h1><p>The requested URL ")
                            .append(req.path)
                            .append(" was not found on this server.</p>");
                        resp.set_body(sb.build(), "text/html");
                    }
                } else {
                    resp.status = HttpStatusCode::METHOD_NOT_ALLOWED;
                    resp.headers["Allow"_s] = get_methods(req.path);
                    resp.set_body("<h1>405 Method Not Allowed</h1>", "text/html");
                }
            }

            // 发送HTTP响应
            send_response(client, resp);
        } catch (const std::exception& ex) {
            io::Log::error("Request error: {}", SRC_LOC, ex.what());
            try {
                send_error_response(client, HttpStatusCode::INTERNAL_SERVER_ERROR);
            } catch (const std::exception& inner_ex) {
                io::Log::error("Failed to send error response: {}", SRC_LOC, inner_ex.what());
            }
        }
    }

    /**
     * @brief 解析HTTP请求
     * @param client 客户端
     * @return HTTP请求
     */
    HttpRequest parse_request(Socket& client) {
        HttpRequest req;

        // 读取请求行
        auto line = read_line(client);
        if (line.empty()) {
            throw runtime_exception("Empty request");
        }
        io::Log::debug("Request line: {}", SRC_LOC, line);

        // 解析请求行
        auto parts = line.split(" "_s);
        if (parts.size() < 3) {
            throw runtime_exception("Invalid request line: {}", line);
        }

        req.method = parse_method(parts[0]);
        req.path = parts[1];
        req.version = parts[2];

        // 解析查询参数
        parse_query_params(req);

        // 读取头部
        loop {
            auto header_line = read_line(client);
            if (header_line.empty()) break; // 空行结束头部

            io::Log::debug("Header: {}", SRC_LOC, header_line);

            auto colon_pos = header_line.find(util::CodePoint(':'));
            if (colon_pos == npos) {
                throw runtime_exception("Invalid header: {}", header_line);
            }

            auto key = header_line.slice(0, colon_pos).trim();
            auto value = header_line.slice(colon_pos + 1).trim();
            req.headers[key.to_string().lower()] = value;
        }

        // 读取请求体
        if (req.headers.contains("content-length")) {
            try {
                auto content_length = std::stoull(req.headers["content-length"_s].into_string());
                if (content_length > 0 && content_length <= MAX_BODY_SIZE) {
                    req.body = client.recv_bytes(content_length);
                    io::Log::debug("Request body: {}", SRC_LOC, req.body);
                }
            } catch (const std::exception& e) {
                io::Log::error("Invalid Content-Length: {}", SRC_LOC, e.what());
            }
        }

        return req;
    }

    /**
     * @brief 读取一行数据
     */
    static util::String read_line(const Socket& client) {
        util::StringBuilder line;
        char c;

        loop {
            // 逐字符读取
            auto recv = client.recv_bytes(1);
            if (recv.empty()) break;

            c = recv[0];
            if (c == '\n') break;
            if (c != '\r') line.append(c);
        }

        return line.build();
    }

    /**
     * @brief 解析HTTP方法
     * @param method_str 方法字符串
     * @return HttpMethod枚举值
     */
    static HttpMethod parse_method(const util::String& method_str) {
        static const util::Dict<util::String, HttpMethod> method_map = {
            {"GET", HttpMethod::GET},
            {"POST", HttpMethod::POST},
            {"PUT", HttpMethod::PUT},
            {"DELETE", HttpMethod::DELETE},
            {"CONNECT", HttpMethod::CONNECT},
            {"OPTIONS", HttpMethod::OPTIONS},
            {"TRACE", HttpMethod::TRACE},
            {"PATCH", HttpMethod::PATCH},
            {"HEAD", HttpMethod::HEAD},
        };

        return method_map.get_or_default(method_str.upper(), HttpMethod::UNKNOWN);
    }

    /**
     * @brief 解析查询参数
     * @param req HTTP请求
     */
    static void parse_query_params(HttpRequest& req) {
        auto path_end = req.path.find(util::CodePoint('?'));
        if (path_end == npos) return;

        auto query_str = req.path.slice(path_end + 1);
        req.path = req.path.slice(0, path_end);

        auto params = query_str.split("&"_s);
        for (const auto& param : params) {
            auto eq_pos = param.find(util::CodePoint('='));
            if (eq_pos != npos) {
                auto key = param.slice(0, eq_pos);
                auto value = param.slice(eq_pos + 1);
                req.query_params[key] = value;
            } else if (!param.empty()) {
                req.query_params[param] = "";
            }
        }
    }

    /**
     * @brief 处理静态文件请求
     * @param req HTTP请求
     * @param client 客户端
     * @return 是否处理成功
     */
    bool handle_static_file(const HttpRequest& req, const Socket& client) {
        // 只处理GET和HEAD请求
        if (req.method != HttpMethod::GET && req.method != HttpMethod::HEAD) {
            return false;
        }

        for (const auto& [url_prefix, config] : static_dirs_) {
            if (req.path.starts_with(url_prefix)) {
                // 构建文件系统路径
                util::String fs_path = config.fspath + req.path.slice(url_prefix.length());

                // 防止路径遍历攻击
                if (fs_path.find(".."_s) != npos) {
                    io::Log::error("Path traversal detected: {}", SRC_LOC, fs_path);
                    send_error_response(client, HttpStatusCode::FORBIDDEN);
                    return true;
                }

                try {
                    // 检查文件是否存在
                    if (!fs::win::exists(fs_path.__str__())) {
                        send_error_response(client, HttpStatusCode::NOT_FOUND);
                        return true;
                    }

                    // 打开文件
                    fs::win::File file(fs_path.__str__(), "r"); // TODO 改成rb

                    // 读取文件内容
                    auto content = file.read();

                    // 构建响应
                    HttpResponse resp;
                    resp.status = HttpStatusCode::OK;

                    // 设置MIME类型
                    auto ext_pos = fs_path.find_last_of('.');
                    if (ext_pos != npos) {
                        auto ext = fs_path.slice(ext_pos + 1).to_string().lower();
                        if (mime_types_.contains(ext)) {
                            resp.set_content_type(mime_types_[ext]);
                        } else {
                            resp.set_content_type("application/octet-stream");
                        }
                    } else {
                        resp.set_content_type("text/plain");
                    }

                    // 设置缓存头
                    if (config.cache_max_age > 0) {
                        resp.headers["Cache-Control"_s] = "max-age=" + config.cache_max_age;
                    }

                    // 设置内容
                    if (req.method == HttpMethod::GET) {
                        resp.set_body(content);
                    } else { // HEAD请求
                        resp.headers["Content-Length"_s] = util::String::from_u64(content.length());
                    }

                    // 发送响应
                    send_response(client, resp);
                    return true;
                } catch (const std::exception& e) {
                    io::Log::error("File error: {} - {}", SRC_LOC, fs_path, e.what());
                    send_error_response(client, HttpStatusCode::INTERNAL_SERVER_ERROR);
                    return true;
                }
            }
        }
        return false;
    }

    /**
     * @brief 获取path对应的HTTP方法
     * @param path URL
     * @return 支持的HTTP方法
     */
    util::String get_methods(const util::String& path) {
        util::StringBuilder methods;
        bool first = true;

        for (const auto& [method, routes] : routes_) {
            if (!routes.contains(path)) continue;
            if (!first) methods.append(", ");

            switch (method) {
            case HttpMethod::GET: methods.append("GET"); break;
            case HttpMethod::POST: methods.append("POST"); break;
            case HttpMethod::PUT: methods.append("PUT"); break;
            case HttpMethod::DELETE: methods.append("DELETE"); break;
            case HttpMethod::CONNECT: methods.append("CONNECT"); break;
            case HttpMethod::OPTIONS: methods.append("OPTIONS"); break;
            case HttpMethod::TRACE: methods.append("TRACE"); break;
            case HttpMethod::PATCH: methods.append("PATCH"); break;
            case HttpMethod::HEAD: methods.append("HEAD"); break;
            default: break;
            }

            first = false;
        }

        return methods.build();
    }

private:
    TcpServer server_;                                                       // TCP服务器
    async::ThreadPool pool_;                                                 // 线程池
    util::Dict<HttpMethod, util::Dict<util::String, RouterHandler>> routes_; // 路由表
    util::Dict<util::String, StaticDirConfig> static_dirs_;                  // 静态文件配置
    util::Dict<util::String, util::String> mime_types_;                      // MIME类型映射
    std::atomic<u32> active_connections_{0};                                 // 当前活跃连接数
    u32 max_connections_{0};                                                 // 最大连接数
    u32 timeout_{30};                                                        // 超时时间（秒）

    mutable std::mutex routes_mutex_;              // 路由表互斥锁
    static constexpr usize MAX_HEADER_SIZE = 8192; // 最大请求头
    static constexpr usize MAX_BODY_SIZE = 65536;  // 最大请求体
};

} // namespace my::net

namespace std {

// 为 HttpStatusCode 提供格式化支持
template <>
struct formatter<my::net::HttpStatusCode> : std::formatter<int> {
    auto format(my::net::HttpStatusCode code, std::format_context& ctx) const {
        return std::formatter<int>::format(static_cast<int>(code), ctx);
    }
};

// 为 HttpMethod 提供格式化支持
template <>
struct formatter<my::net::HttpMethod> : std::formatter<std::string_view> {
    auto format(my::net::HttpMethod method, std::format_context& ctx) const {
        std::string_view str = "UNKNOWN";
        switch (method) {
        case my::net::HttpMethod::GET: str = "GET"; break;
        case my::net::HttpMethod::POST: str = "POST"; break;
        case my::net::HttpMethod::PUT: str = "PUT"; break;
        case my::net::HttpMethod::DELETE: str = "DELETE"; break;
        case my::net::HttpMethod::CONNECT: str = "CONNECT"; break;
        case my::net::HttpMethod::OPTIONS: str = "OPTIONS"; break;
        case my::net::HttpMethod::TRACE: str = "TRACE"; break;
        case my::net::HttpMethod::PATCH: str = "PATCH"; break;
        case my::net::HttpMethod::HEAD: str = "HEAD"; break;
        default: break;
        }
        return std::formatter<std::string_view>::format(str, ctx);
    }
};

} // namespace std

#endif // HTTP_HPP