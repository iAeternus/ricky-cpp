/**
 * @brief HTTP服务器
 * @author Ricky
 * @date 2025/8/13
 * @version 1.0
 */
#include "Log.hpp"
#include "http.hpp"

constexpr auto IP = "127.0.0.1";
constexpr auto PORT = 8080;
constexpr auto NUM_OF_THREADS = 100;
constexpr auto STATIC_FILE_PATH = R"(F:\Develop\cpp\ricky-cpp\tests\resources\text.txt)";

void example_http_server() {
    using namespace my;

    net::HttpServer server(IP, PORT, NUM_OF_THREADS);
    io::Log::set_level(io::Log::LogLevel::TRACE_);

    /**
     * 静态文件服务
     * http://127.0.0.1:8080/static
     */
    server.static_file("/static", STATIC_FILE_PATH, 3600);

    /**
     * 首页
     * http://127.0.0.1:8080/
     */
    server.get("/", [](const net::HttpRequest& req) {
        net::HttpResponse resp;
        resp.set_body("<h1>Welcome to Ricky-Cpp HTTP Server</h1>", "text/html");
        return resp;
    });

    /**
     * 问候页面
     * http://127.0.0.1:8080/hello?name=Ricky
     */
    server.get("/hello", [](const net::HttpRequest& req) {
        net::HttpResponse resp;
        const auto name = req.query_params.get_or_default("name"_s, "Guest");
        resp.set_body("Hello, "_s + name + "!"_s, "text/plain");
        return resp;
    });

    /**
     * 表单提交，使用cmd测试
     * curl -v -X POST -d "test data" http://127.0.0.1:8080/submit
     */
    server.post("/submit", [](const net::HttpRequest& req) {
        net::HttpResponse resp;
        resp.status = net::HttpStatusCode::CREATED;
        resp.set_body("Received: "_s + req.body, "text/plain");
        return resp;
    });

    /**
     * 重定向到 /new TODO 存在问题
     * http://127.0.0.1:8080/old
     */
    server.get("/old", [](const net::HttpRequest& req) {
        net::HttpResponse resp;
        resp.set_redirect("/new");
        return resp;
    });

    server.get("/new", [](const net::HttpRequest& req) {
        net::HttpResponse resp;
        resp.set_body("This is the new page", "text/plain");
        return resp;
    });

    server.start();
}

int main() {
    system(("chcp " + std::to_string(CP_UTF8)).c_str()); // 控制台输出ASC颜色字符
    example_http_server();
}