/**
 * @brief HTTP服务器，提供HTTP/1.1协议支持
 * @author Ricky
 * @date 2025/7/14
 * @version 1.0
 */
#ifndef HTTP_HPP
#define HTTP_HPP

#include "Dict.hpp"
#include "tcp.hpp"

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
    SERVICE_UNAVAILABLE = 503
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
    {HttpStatusCode::SERVICE_UNAVAILABLE, "Service Unavailable"}};

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
        return std::stoul(content_length);
    }

private:
    HttpMethod method_ = HttpMethod::UNKNOWN;       // 请求方法
    CString path_;                                  // 请求路径
    CString version_;                               // HTTP版本
    util::Dict<CString, std::string> headers_;      // 请求头
    std::string body_;                              // 请求体
    util::Dict<CString, std::string> query_params_; // 查询参数
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
     * @brief 设置响应头中的 Content-Type
     */
    void set_content_type(const std::string& type) {
        headers_["Content-Type"] = type;
    }

    /**
     * @brief 设置响应体
     */
    void set_body(const std::string& content, const std::string& type = "text/plain") {
        body_ = content;
        set_content_type(type);
        headers_["Content-Length"] = std::to_string(body_.length());
    }

    /**
     * @brief 设置重定向
     */
    void set_redirect(const std::string& location, HttpStatusCode code = HttpStatusCode::FOUND) {
        status_ = code;
        headers_["Location"] = location;
        body_.clear();
        headers_.remove("Content-Length");
    }

private:
    HttpStatusCode status_ = HttpStatusCode::OK; // 响应状态
    util::Dict<CString, std::string> headers_;   // 响应头
    std::string body_;                           // 响应体
};

/**
 * @brief 路由处理函数类型
 */
using RouterHandler = std::function<HttpResponse(const HttpRequest&)>;

class HttpServer : public Object<HttpServer> {
public:
    using Self = HttpServer;

    /**
     * @brief 构造函数
     * @param ip 监听IP地址
     * @param port 监听端口
     * @param maxConnections 最大并发连接数，0表示无限制
     */
    HttpServer(const char* ip, u16 port, u32 max_connections = 0) :
            server_(ip, port), max_connections_(max_connections) {}

private:
    /**
     * @brief 静态路径配置
     */
    struct StaticDirConfig {
        std::string fspath;    // 文件系统路径
        u32 cache_max_age = 0; // 缓存时间（秒）
    };

private:
    TcpServer server_;                                                      // TCP服务器
    util::Dict<HttpMethod, util::Dict<std::string, RouterHandler>> routes_; // 路由表
    util::Dict<std::string, StaticDirConfig> static_dirs_;                  // 静态路径表 TODO 命名
    std::atomic<u32> active_connections_{0};                                // 当前活跃连接数
    u32 max_connections_{0};                                                // 最大连接数
};

} // namespace my::net

#endif // HTTP_HPP