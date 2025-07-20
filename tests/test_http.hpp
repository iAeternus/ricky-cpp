#ifndef TEST_HTTP
#define TEST_HTTP

#include "Log.hpp"
#include "http.hpp"
#include "UnitTest.hpp"

namespace my::test::test_http {

fn it_works = []() {
    net::HttpServer server("127.0.0.1", 8080, 100);
    io::Log::set_level(io::Log::LogLevel::TRACE_);

    /*
    静态文件服务
    http://127.0.0.1:8080/static
    */
    server.static_file("/static", "F:\\develop\\ricky-cpp\\tests\\resources\\text.txt", 3600);

    /*
    首页
    http://127.0.0.1:8080/
    */
    server.get("/", [](const net::HttpRequest& req) {
        net::HttpResponse resp;
        resp.set_body("<h1>Welcome to Ricky-Cpp HTTP Server</h1>", "text/html");
        return resp;
    });

    /*
    问候页面
    http://127.0.0.1:8080/hello?name=Ricky
    */
    server.get("/hello", [](const net::HttpRequest& req) {
        net::HttpResponse resp;
        auto name = req.query_params.get_or_default("name"_s, "Guest");
        resp.set_body("Hello, "_s + name + "!"_s, "text/plain");
        return resp;
    });

    /*
    表单提交，使用cmd测试
    curl -v -X POST -d "test data" http://127.0.0.1:8080/submit
    */
    server.post("/submit", [](const net::HttpRequest& req) {
        net::HttpResponse resp;
        resp.status = net::HttpStatusCode::CREATED;
        resp.set_body("Received: "_s + req.body, "text/plain");
        return resp;
    });

    /*
    重定向到 /new TODO 存在问题
    http://127.0.0.1:8080/old
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
};

fn test_http() {
    UnitTestGroup group{"test_http"};

    group.addTest("it_works", it_works);

    group.startAll();
}

} // namespace my::test::test_http

#endif // TEST_HTTP