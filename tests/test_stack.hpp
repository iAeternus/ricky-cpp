#ifndef TEST_STACK_HPP
#define TEST_STACK_HPP

#include "ricky_test.hpp"
#include "Stack.hpp"

namespace my::test::test_stack {

auto it_works = []() {
    util::Stack<i32> st;
    Assertions::assertTrue(st.isEmpty());

    st.push(1), st.push(2), st.push(3);
    Assertions::assertFalse(st.isEmpty());
    Assertions::assertEquals(3LL, st.size());
    Assertions::assertTrue(3, st.peek());

    st.pop();
    Assertions::assertEquals(2LL, st.size());
    Assertions::assertEquals(2, st.peek());

    st.pop(), st.pop();
    Assertions::assertTrue(st.isEmpty());
    Assertions::assertEquals(0LL, st.size());
};

void test_stack() {
    UnitTestGroup group{"test_stack"};

    group.addTest("it_works", it_works);

    group.startAll();
}

} // namespace my::test::test_stack

#endif // TEST_STACK_HPP