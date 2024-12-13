#ifndef TEST_RICKY_HPP
#define TEST_RICKY_HPP

#include "ricky_test.hpp"
#include "ricky.hpp"

using namespace my;

auto size_should_correct = []() {
    test::Assertions::assertEquals(1, int(sizeof(i8)));
    test::Assertions::assertEquals(1, int(sizeof(u8)));
    test::Assertions::assertEquals(2, int(sizeof(i16)));
    test::Assertions::assertEquals(2, int(sizeof(u16)));
    test::Assertions::assertEquals(4, int(sizeof(i32)));
    test::Assertions::assertEquals(4, int(sizeof(u32)));
    test::Assertions::assertEquals(8, int(sizeof(i64)));
    test::Assertions::assertEquals(8, int(sizeof(i64)));
};

void test_ricky() {
    test::UnitTestGroup group("test_ricky");

    group.addTest("size_should_correct", size_should_correct);

    group.startAll();
}

#endif // TEST_RICKY_HPP