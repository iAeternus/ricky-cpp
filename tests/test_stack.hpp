#ifndef TEST_STACK_HPP
#define TEST_STACK_HPP

#include "UnitTest.hpp"
#include "Assertions.hpp"
#include "Stack.hpp"

namespace my::test::test_stack {

fn it_works = []() {
    util::Stack<i32> st;
    Assertions::assertTrue(st.empty());

    st.push(1), st.push(2), st.push(3);
    Assertions::assertFalse(st.empty());
    Assertions::assertEquals(3, st.size());
    Assertions::assertTrue(3, st.peek());

    st.pop();
    Assertions::assertEquals(2, st.size());
    Assertions::assertEquals(2, st.peek());

    st.pop(), st.pop();
    Assertions::assertTrue(st.empty());
    Assertions::assertEquals(0, st.size());
};

fn should_fail_to_peek_if_stack_is_empty = []() {
    // Given
    util::Stack<i32> st;

    // When & Then
    Assertions::assertThrows("stack is empty.", [&]() {
        st.peek();
    });
};

fn test_stack() {
    UnitTestGroup group{"test_stack"};

    group.addTest("it_works", it_works);
    group.addTest("should_fail_to_peek_if_stack_is_empty", should_fail_to_peek_if_stack_is_empty);

    group.startAll();
}

} // namespace my::test::test_stack

#endif // TEST_STACK_HPP