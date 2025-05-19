#ifndef TEST_LOG_HPP
#define TEST_LOG_HPP

#include "ricky_test.hpp"
#include "Log.hpp"

namespace my::test::test_log {

using namespace my::io;

fn it_works = []() {
    Log::setLevel(io::Log::LogLevel::INFO_);
    Log::trace("This is a trace log.");
    Log::debug("This is a debug log.");
    Log::info("This is an info log.");
    Log::warn("This is a warn log.");
    Log::error("This is an error log.");
    Log::fatal("This is a fatal log.");

    io::println("-----------------------------");

    Log::setLevel(io::Log::LogLevel::INFO_);
    Log::addHandler(io::Log::LogHandler(io::Log::LogLevel::FATAL_, stdout, false));
    Log::trace("This is a trace log.");
    Log::debug("This is a debug log.");
    Log::info("This is an info log.");
    Log::warn("This is a warn log.");
    Log::error("This is an error log.");
    Log::fatal("This is a fatal log.");
};

fn test_log() {
    UnitTestGroup group{"test_log"};

    group.addTest("it_works", it_works);

    group.startAll();
}

} // namespace my::test::test_log

#endif // TEST_LOG_HPP