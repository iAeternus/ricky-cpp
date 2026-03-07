/**
 * @brief Huffman tree
 * @author Ricky
 * @date 2025/5/11
 * @version 1.0
 */
#ifndef HUFFMAN_TREE_HPP
#define HUFFMAN_TREE_HPP

#include "string.hpp"
#include "hash_map.hpp"
#include "binary_heap.hpp"
#include "marker.hpp"

namespace my::util {

class HuffmanTree : public Object<HuffmanTree>, public NoCopyMove {
public:
    using Self = HuffmanTree;
    using String = str::String<>;
    static constexpr usize NIL = -1ULL;

    struct Node {
        usize idx;
        char32_t data;
        u32 freq;
        usize lch = NIL, rch = NIL;

        Node(const usize idx, char32_t data, const u32 freq) :
                idx(idx), data(data), freq(freq) {}

        Node(const usize idx, const u32 freq, const usize lch, const usize rch) :
                idx(idx), freq(freq), lch(lch), rch(rch) {}

        bool operator<(const Node& other) const {
            return this->freq < other.freq;
        }
    };

    explicit HuffmanTree(const String& text) :
            text_(text) {
        for (const auto cp : text.chars()) {
            ++freqs_[to_key(cp)];
            ++char_count_;
        }

        usize idx = 0;
        BinaryHeap<Node> bh;
        for (const auto& [cp, freq] : freqs_) {
            nodes_.push(idx++, cp, freq);
            bh.push(nodes_.last());
        }

        build_tree(bh);
        generate_key();
    }

    String encode() {
        String encoded;
        for (const auto cp : text_.chars()) {
            const auto key = to_key(cp);
            if (!key_.contains(key)) {
                throw runtime_exception("missing code for character");
            }
            encoded.push_str(key_.get(key).as_str());
        }
        encoded_text_ = std::move(encoded);
        return encoded_text_;
    }

    String decode() {
        String decoded;
        usize cur = root_idx_;
        for (const auto b : encoded_text_.bytes()) {
            const auto bit = static_cast<char>(b);
            if (bit != '0' && bit != '1') {
                throw runtime_exception("invalid encoded stream");
            }
            cur = (bit == '0') ? nodes_[cur].lch : nodes_[cur].rch;
            if (cur == NIL) throw runtime_exception("invalid encoded stream");

            if (nodes_[cur].lch == NIL && nodes_[cur].rch == NIL) {
                decoded.push(nodes_[cur].data);
                cur = root_idx_;
            }
        }

        if (cur != root_idx_) {
            throw runtime_exception("invalid encoded stream");
        }
        return decoded;
    }

    auto get_key() const {
        return key_;
    }

    auto get_freqs() const {
        return freqs_;
    }

    u32 wpl() const {
        u32 total = 0;
        for (const auto& [cp, code] : key_) {
            if (freqs_.contains(cp)) {
                total += freqs_.get(cp) * code.len();
            }
        }
        return total;
    }

    f64 acl() const {
        if (freqs_.empty()) return 0.0;
        return wpl() / static_cast<f64>(char_count_);
    }

    usize height() const {
        if (root_idx_ == NIL) throw runtime_exception("please build the tree first.");
        return height(root_idx_);
    }

private:
    void build_tree(BinaryHeap<Node>& bh) {
        if (bh.is_empty()) return;

        if (bh.size() == 1) {
            Node dummy{nodes_.len(), 0, bh.top().idx, NIL};
            nodes_.push(dummy);
            root_idx_ = dummy.idx;
            return;
        }

        while (bh.size() > 1) {
            auto left = bh.top();
            bh.pop();
            auto right = bh.top();
            bh.pop();

            if (left.freq == right.freq && left.idx > right.idx) {
                std::swap(left, right);
            }

            Node parent(nodes_.len(), left.freq + right.freq, left.idx, right.idx);
            nodes_.push(parent);
            bh.push(parent);
        }
        root_idx_ = bh.top().idx;
    }

    void generate_key() {
        if (root_idx_ == NIL) return;
        String code;
        traverse(root_idx_, code);
    }

    void traverse(const usize cur, String& code) {
        if (cur == NIL) return;
        const Node& node = nodes_[cur];

        if (node.lch == NIL && node.rch == NIL) {
            key_[to_key(node.data)] = code;
            return;
        }

        if (node.lch != NIL) {
            code.push(U'0');
            traverse(node.lch, code);
            code.pop();
        }

        if (node.rch != NIL) {
            code.push(U'1');
            traverse(node.rch, code);
            code.pop();
        }
    }

    usize height(const usize cur_idx) const {
        if (cur_idx == NIL) return 0;
        const Node& node = nodes_[cur_idx];
        return 1 + std::max(height(node.lch), height(node.rch));
    }

private:
    static constexpr u32 to_key(const char32_t cp) {
        return static_cast<u32>(cp);
    }

private:
    str::String<> text_;
    str::String<> encoded_text_;
    Vec<Node> nodes_;
    HashMap<u32, str::String<>> key_;
    HashMap<u32, u32> freqs_;
    u32 char_count_ = 0;
    usize root_idx_ = NIL;
};

} // namespace my::util

#endif // HUFFMAN_TREE_HPP
