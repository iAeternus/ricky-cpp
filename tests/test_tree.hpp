#ifndef TEST_TREE_HPP
#define TEST_TREE_HPP

#include "TreeNode.hpp"
#include "UnitTest.hpp"
#include "Assertions.hpp"
#include "Tree.hpp"

namespace my::test::test_tree {

fn it_works = []() {
    util::Tree<i32> t;
    auto* root = t.set_root(1);

    t.add_child(root, 2);
    t.add_child(root, 3);
    t.add_child(t.root(), 4);
    t.add_child(root->children_[0], 5);

    io::println(t);
};

fn should_dfs = []() {
    // Given
    util::Tree<i32> t;
    t.set_root(1);
    t.add_child(t.root(), 2);
    t.add_child(t.root(), 3);
    t.add_child(t.root(), 4);
    t.add_child(t.root()->children_[0], 5);
    util::DynArray<i32> res;

    // When
    t.dfs([&res](const auto& v) {
        res.append(v);
    });

    // Then
    Assertions::assertEquals("[1,2,5,3,4]"_cs, res.__str__());
};

fn should_bfs = []() {
    // Given
    util::Tree<i32> t;
    t.set_root(1);
    t.add_child(t.root(), 2);
    t.add_child(t.root(), 3);
    t.add_child(t.root(), 4);
    t.add_child(t.root()->children_[0], 5);
    util::DynArray<i32> res;

    // When
    t.bfs([&res](const auto& v) {
        res.append(v);
    });

    // Then
    Assertions::assertEquals("[1,2,3,4,5]"_cs, res.__str__());
};

fn test_tree() {
    UnitTestGroup group{"test_tree"};

    group.addTest("it_works", it_works);
    group.addTest("should_dfs", should_dfs);
    group.addTest("should_bfs", should_bfs);

    group.startAll();
}

} // namespace my::test::test_tree

#endif // TEST_TREE_HPP