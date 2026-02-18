#include "test_tree.hpp"
#include "tree.hpp"
#include "printer.hpp"
#include "ricky_test.hpp"

namespace my::test::test_tree {

void it_works() {
    util::Tree<i32> t;
    auto* root = t.set_root(1);

    t.add_child(root, 2);
    t.add_child(root, 3);
    t.add_child(t.root(), 4);
    t.add_child(root->subs_[0], 5);

    io::println(t);
}

void should_dfs() {
    // Given
    util::Tree<i32> t;
    t.set_root(1);
    t.add_child(t.root(), 2);
    t.add_child(t.root(), 3);
    t.add_child(t.root(), 4);
    t.add_child(t.root()->subs_[0], 5);
    util::Vec<i32> res;

    // When
    t.dfs([&res](const auto& v) {
        res.push(v);
    });

    // Then
    Assertions::assertEquals("[1,2,5,3,4]"_cs, res.__str__());
}

void should_bfs() {
    // Given
    util::Tree<i32> t;
    t.set_root(1);
    t.add_child(t.root(), 2);
    t.add_child(t.root(), 3);
    t.add_child(t.root(), 4);
    t.add_child(t.root()->subs_[0], 5);
    util::Vec<i32> res;

    // When
    t.bfs([&res](const auto& v) {
        res.push(v);
    });

    // Then
    Assertions::assertEquals("[1,2,3,4,5]"_cs, res.__str__());
}

GROUP_NAME("test_tree")
REGISTER_UNIT_TESTS(
    UNIT_TEST_ITEM(it_works),
    UNIT_TEST_ITEM(should_dfs),
    UNIT_TEST_ITEM(should_bfs))

} // namespace my::test::test_tree