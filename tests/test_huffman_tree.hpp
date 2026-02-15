#ifndef TEST_HUFFMAN_TREE_HPP
#define TEST_HUFFMAN_TREE_HPP

#include "ricky_test.hpp"
#include "filesystem.hpp"
#include "huffman_tree.hpp"

namespace my::test::test_huffman_tree {

static constexpr auto CLASS_PATH = R"(F:\Develop\cpp\ricky-cpp\tests\resources)";

auto it_works = []() {
    // Given
    const char* path = fs::win::join(CLASS_PATH, "text.txt");
    fs::win::File file{path, "r"};
    util::String text = util::String(file.read().data());

    // When
    util::HuffmanTree h(text);
    auto encode_res = h.encode();

    // Then
    Assertions::assertTrue(!encode_res.is_empty());

    // When
    auto decode_res = h.decode();

    // Then
    Assertions::assertEquals(text, decode_res);
};

auto should_handle_empty_string = []() {
    // Given
    util::String text = ""_s;

    // When
    util::HuffmanTree h(text);
    auto encode_res = h.encode();

    // Then
    Assertions::assertTrue(encode_res.is_empty());
    Assertions::assertEquals(0ULL, h.wpl());
    Assertions::assertEquals(0.0, h.acl());

    // When
    auto decode_res = h.decode();

    // Then
    Assertions::assertTrue(decode_res.is_empty());
};

auto should_handle_non_ascii_character = []() {
    // Given
    util::String text = "你好好";

    // When
    util::HuffmanTree h(text);
    auto encode_res = h.encode();

    // Then
    Assertions::assertEquals("011"_s, encode_res);
    Assertions::assertEquals(text, h.decode());
};

auto test_huffman_tree() {
    UnitTestGroup group{"test_huffman_tree"};

    group.addTest("it_works", it_works);
    group.addTest("should_handle_empty_string", should_handle_empty_string);
    group.addTest("should_handle_non_ascii_character", should_handle_non_ascii_character);

    group.startAll();
}

} // namespace my::test::test_huffman_tree

#endif // TEST_HUFFMAN_TREE_HPP