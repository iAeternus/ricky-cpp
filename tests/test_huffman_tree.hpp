#ifndef TEST_HUFFMAN_TREE_HPP
#define TEST_HUFFMAN_TREE_HPP

#include "ricky_test.hpp"
#include "filesystem.hpp"
#include "HuffmanTree.hpp"

namespace my::test::test_huffman_tree {

static constexpr const char* CLASS_PATH = "F:\\develop\\ricky-cpp\\tests\\resources";

auto it_works = []() {
    // Given
    const char* path = fs::win::join(CLASS_PATH, "text.txt");
    fs::win::File file{path, "r"};
    util::String text = util::String(file.read().data());

    // When
    util::HuffmanTree h(text);
    auto encode_res = h.encode();

    // Then
    const char* path2 = fs::win::join(CLASS_PATH, "code.txt");
    fs::win::File file2{path2, "r"};

    Assertions::assertEquals(util::String(file2.read().data()), encode_res);
    Assertions::assertEquals(2359, h.wpl());
    Assertions::assertEquals(4.510516252390057, h.acl());
    Assertions::assertEquals(10, h.height());

    // When
    auto decode_res = h.decode();

    // Then
    Assertions::assertEquals(text, decode_res);
};

void test_huffman_tree() {
    UnitTestGroup group{"test_huffman_tree"};

    group.addTest("it_works", it_works);

    group.startAll();
}

} // namespace my::test::test_huffman_tree

#endif // TEST_HUFFMAN_TREE_HPP