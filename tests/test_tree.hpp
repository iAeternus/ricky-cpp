#ifndef TEST_TREE_HPP
#define TEST_TREE_HPP

#include "ricky_test.hpp"
#include "Tree.hpp"

namespace my::test::test_tree {

auto it_works = []() {
    util::Tree<i32> t;
    auto* root = t.setRoot(1);

    t.addChild(root, 2);
    t.addChild(root, 3);
    t.addChild(t.root(), 4);
    t.addChild(root->children_[0], 5);

    io::println(t);
};

auto should_dfs = []() {
    // Given
    util::Tree<i32> t;
    t.setRoot(1);
    t.addChild(t.root(), 2);
    t.addChild(t.root(), 3);
    t.addChild(t.root(), 4);
    t.addChild(t.root()->children_[0], 5);
    util::DynArray<i32> res;

    // When
    t.dfs([&res](const auto& v) {
        res.append(v);
    });

    // Then
    Assertions::assertEquals("[1,2,5,3,4]"_cs, res.__str__());
};

auto should_bfs = []() {
    // Given
    util::Tree<i32> t;
    t.setRoot(1);
    t.addChild(t.root(), 2);
    t.addChild(t.root(), 3);
    t.addChild(t.root(), 4);
    t.addChild(t.root()->children_[0], 5);
    util::DynArray<i32> res;

    // When
    t.bfs([&res](const auto& v) {
        res.append(v);
    });

    // Then
    Assertions::assertEquals("[1,2,3,4,5]"_cs, res.__str__());
};

void test_tree() {
    UnitTestGroup group{"test_tree"};
    
    group.addTest("it_works", it_works);
    group.addTest("should_dfs", should_dfs);
    group.addTest("should_bfs", should_bfs);

    group.startAll();
}

} // namespace my::test::test_tree

#endif // TEST_TREE_HPP