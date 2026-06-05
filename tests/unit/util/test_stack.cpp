#include "test_stack.hpp"
#include "vec_stack.hpp"
#include "ricky_test.hpp"

namespace my::test::test_stack {

void it_works() {
    util::Stack<i32> st;
    Assertions::assert_true(st.is_empty());

    st.push(1), st.push(2), st.push(3);
    Assertions::assert_false(st.is_empty());
    Assertions::assert_equals(3, st.size());
    Assertions::assert_equals(3, st.peek());

    st.pop();
    Assertions::assert_equals(2, st.size());
    Assertions::assert_equals(2, st.peek());

    st.pop(), st.pop();
    Assertions::assert_true(st.is_empty());
    Assertions::assert_equals(0, st.size());
}

void should_fail_to_peek_if_stack_is_empty() {
    // Given
    util::Stack<i32> st;

    // When & Then
    Assertions::assert_throws("Stack is is_empty.", [&]() {
        st.peek();
    });
}

GROUP_NAME("test_stack")
REGISTER_UNIT_TESTS(
    UNIT_TEST_ITEM(it_works),
    UNIT_TEST_ITEM(should_fail_to_peek_if_stack_is_empty))

} // namespace my::test::test_stack