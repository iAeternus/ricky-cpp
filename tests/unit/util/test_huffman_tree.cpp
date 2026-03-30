#include "test_huffman_tree.hpp"
#include "file.hpp"
#include "fs.hpp"
#include "huffman_tree.hpp"
#include "ricky_test.hpp"

namespace my::test::test_huffman_tree {

static const fs::PathBuf& res_dir() {
    static const fs::PathBuf root = []() {
        std::string file = __FILE__;
        const char* win_suffix = "\\tests\\unit\\util\\test_huffman_tree.cpp";
        const char* posix_suffix = "/tests/unit/util/test_huffman_tree.cpp";
        auto pos = file.find(win_suffix);
        if (pos == std::string::npos) {
            pos = file.find(posix_suffix);
        }
        if (pos == std::string::npos) {
            return fs::PathBuf(".");
        }
        auto root = fs::PathBuf(file.substr(0, pos).c_str());
        return root.join("tests/resources");
    }();
    return root;
}

void it_works() {
    // Given
    auto path = res_dir().join("text.txt");
    auto file = fs::File::open(path);
    auto text = file.read_all();

    // When
    util::HuffmanTree h(text);
    auto encode_res = h.encode();

    // Then
    Assertions::assertTrue(!encode_res.is_empty());

    // When
    auto decode_res = h.decode();

    // Then
    Assertions::assert_equals(text, decode_res);
}

void should_handle_empty_string() {
    // Given
    str::String<> text;

    // When
    util::HuffmanTree h(text);
    auto encode_res = h.encode();

    // Then
    Assertions::assert_true(encode_res.is_empty());
    Assertions::assert_equals(0ULL, h.wpl());
    Assertions::assert_equals(0.0, h.acl());

    // When
    auto decode_res = h.decode();

    // Then
    Assertions::assert_true(decode_res.is_empty());
}

void should_handle_non_ascii_character() {
    // Given
    str::String<> text("\xE4\xBD\xA0\xE5\xA5\xBD\xE5\xA5\xBD"); // "你好好"

    // When
    util::HuffmanTree h(text);
    auto encode_res = h.encode();

    // Then
    Assertions::assert_equals(str::String<>("011"), encode_res);
    Assertions::assert_equals(text, h.decode());
}

GROUP_NAME("test_huffman_tree")
REGISTER_UNIT_TESTS(
    UNIT_TEST_ITEM(it_works),
    UNIT_TEST_ITEM(should_handle_empty_string),
    UNIT_TEST_ITEM(should_handle_non_ascii_character))

} // namespace my::test::test_huffman_tree
