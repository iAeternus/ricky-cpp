/**
 * @brief
 * @author Ricky
 * @date 2026/2/15
 * @version 1.0
 */
#ifndef TEST_OPTION_AND_RESULT_HPP
#define TEST_OPTION_AND_RESULT_HPP

#include "ricky_test.hpp"
#include "option.hpp"
#include "result.hpp"

namespace my::test::test_option_and_result {

void test_option_basic() {
    my::Option<int> a;
    Assertions::assert_true(a.is_none());

    my::Option<int> b(10);
    Assertions::assert_true(b.is_some());
    Assertions::assert_equals(10, b.unwrap());
}

void test_option_copy_move() {
    my::Option<int> a(42);
    my::Option<int> b = a;
    Assertions::assert_equals(42, b.unwrap());

    my::Option<int> c = std::move(a);
    Assertions::assert_equals(42, c.unwrap());
}

void test_option_unwrap_or() {
    my::Option<int> a;
    my::Option<int> b(5);

    Assertions::assert_equals(100, a.unwrap_or(100));
    Assertions::assert_equals(5, b.unwrap_or(100));
}

void test_option_map() {
    my::Option<int> a(3);
    my::Option<int> b;

    auto r1 = a.map([](int x) { return x * 2; });
    auto r2 = b.map([](int x) { return x * 2; });

    Assertions::assert_true(r1.is_some());
    Assertions::assert_equals(6, r1.unwrap());

    Assertions::assert_true(r2.is_none());
}

void test_option_and_then() {
    my::Option<int> a(4);
    my::Option<int> b;

    auto r1 = a.and_then([](int x) {
        return my::Option<int>(x + 1);
    });

    auto r2 = b.and_then([](int x) {
        return my::Option<int>(x + 1);
    });

    Assertions::assert_true(r1.is_some());
    Assertions::assert_equals(5, r1.unwrap());

    Assertions::assert_true(r2.is_none());
}

// ---------- Result ----------

void test_result_basic() {
    auto ok = my::Result<int, const char*>::Ok(10);
    auto err = my::Result<int, const char*>::Err("fail");

    Assertions::assert_true(ok.is_ok());
    Assertions::assert_equals(10, ok.unwrap());

    Assertions::assert_true(err.is_err());
    Assertions::assert_equals("fail", err.unwrap_err());
}

void test_result_copy_move() {
    auto a = my::Result<int, int>::Ok(7);
    auto b = a;
    Assertions::assert_equals(7, b.unwrap());

    auto c = std::move(a);
    Assertions::assert_equals(7, c.unwrap());
}

void test_result_map() {
    auto ok = my::Result<int, const char*>::Ok(5);
    auto err = my::Result<int, const char*>::Err("bad");

    auto r1 = ok.map([](int x) { return x * 2; });
    auto r2 = err.map([](int x) { return x * 2; });

    Assertions::assert_true(r1.is_ok());
    Assertions::assert_equals(10, r1.unwrap());

    Assertions::assert_true(r2.is_err());
    Assertions::assert_equals("bad", r2.unwrap_err());
}

void test_result_map_err() {
    auto ok = my::Result<int, int>::Ok(1);
    auto err = my::Result<int, int>::Err(404);

    auto r1 = ok.map_err([](int e) { return e + 1; });
    auto r2 = err.map_err([](int e) { return e + 1; });

    Assertions::assert_true(r1.is_ok());
    Assertions::assert_equals(1, r1.unwrap());

    Assertions::assert_true(r2.is_err());
    Assertions::assert_equals(405, r2.unwrap_err());
}

void test_result_and_then() {
    auto ok = my::Result<int, const char*>::Ok(3);
    auto err = my::Result<int, const char*>::Err("oops");

    auto r1 = ok.and_then([](int x) {
        return my::Result<int, const char*>::Ok(x * 3);
    });

    auto r2 = err.and_then([](int x) {
        return my::Result<int, const char*>::Ok(x * 3);
    });

    Assertions::assert_true(r1.is_ok());
    Assertions::assert_equals(9, r1.unwrap());

    Assertions::assert_true(r2.is_err());
    Assertions::assert_equals("oops", r2.unwrap_err());
}

void test_result_unwrap_or() {
    auto ok = my::Result<int, int>::Ok(8);
    auto err = my::Result<int, int>::Err(99);

    Assertions::assert_equals(8, ok.unwrap_or(100));
    Assertions::assert_equals(100, err.unwrap_or(100));
}

void test_option_and_result() {
    UnitTestGroup group{"test_option_and_result"};

    group.addTest("option_basic", test_option_basic);
    group.addTest("option_copy_move", test_option_copy_move);
    group.addTest("option_unwrap_or", test_option_unwrap_or);
    group.addTest("option_map", test_option_map);
    group.addTest("option_and_then", test_option_and_then);

    group.addTest("result_basic", test_result_basic);
    group.addTest("result_copy_move", test_result_copy_move);
    group.addTest("result_map", test_result_map);
    group.addTest("result_map_err", test_result_map_err);
    group.addTest("result_and_then", test_result_and_then);
    group.addTest("result_unwrap_or", test_result_unwrap_or);

    group.startAll();
}

} // namespace my::test::test_option_and_result

#endif // TEST_OPTION_AND_RESULT_HPP
