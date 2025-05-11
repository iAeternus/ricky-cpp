/**
 * @brief 哈夫曼树
 * @author Ricky
 * @date 2025/5/11
 * @version 1.0
 */
#ifndef HUFFMAN_TREE_HPP
#define HUFFMAN_TREE_HPP

#include "String.hpp"
#include "StringBuilder.hpp"
#include "Dict.hpp"
#include "Vec.hpp"

#include <queue>

namespace my::util {

class HuffmanTree : public Object<HuffmanTree>, public NoCopy {
public:
    using Self = HuffmanTree;
    static constexpr isize NIL = -1;

    struct Node {
        isize idx;
        CodePoint data;
        u32 freq;
        isize lch = NIL, rch = NIL;

        Node(isize idx, const CodePoint& data, u32 freq) :
                idx(idx), data(data), freq(freq) {}

        Node(isize idx, u32 freq, isize lch, isize rch) :
                idx(idx), freq(freq), lch(lch), rch(rch) {}

        bool operator<(const Node& other) const {
            return this->freq > other.freq;
        }
    };

    HuffmanTree(const String& text) :
            text_(text) {
        for (const auto& cp : text) {
            ++freqs_[cp];
        }

        isize idx = 0;
        std::priority_queue<Node> pq;
        for (const auto& [cp, freq] : freqs_) {
            nodes_.append(idx++, cp, freq);
            pq.push(nodes_.back());
        }

        build_tree(pq);
        generate_key();
    }

    String encode() {
        StringBuilder sb;
        for (const auto& cp : text_) {
            if (!key_.contains(cp)) {
                RuntimeError("Missing code for character");
            }
            sb.append(key_[cp].data());
        }
        encoded_text_ = sb.str();
        return encoded_text_;
    }

    String decode() {
        StringBuilder sb;
        isize cur = root_idx_;
        for (char bit : encoded_text_) {
            cur = (bit == '0') ? nodes_[cur].lch : nodes_[cur].rch;
            if (cur == NIL) RuntimeError("Invalid encoded stream");

            if (nodes_[cur].lch == NIL && nodes_[cur].rch == NIL) {
                sb.append(nodes_[cur].data);
                cur = root_idx_;
            }
        }
        return sb.str();
    }

    Dict<CodePoint, CString> get_key() const {
        return key_;
    }

    Dict<CodePoint, u32> get_freqs() const {
        return freqs_;
    }

    /**
     * @brief 计算带权路径长度
     * @note Weighted Path Length
     */
    u32 wpl() const {
        u32 total = 0;
        for (const auto& [cp, code] : key_) {
            if (freqs_.contains(cp)) {
                total += freqs_.get(cp) * code.size();
            }
        }
        return total;
    }

    /**
     * @brief 计算平均编码长度
     * @note Average Code Length
     */
    f64 acl() const {
        if (freqs_.empty()) return 0.0;
        return wpl() / f64(text_.length());
    }

    /**
     * @brief 计算树高
     */
    isize height() const {
        if (root_idx_ == NIL) RuntimeError("Please build the tree first.");
        return height(root_idx_);
    }

private:
    String text_;                  // 原始文本
    String encoded_text_;          // 编码后的文本
    Vec<Node> nodes_;              // 树结构
    Dict<CodePoint, CString> key_; // 密钥
    Dict<CodePoint, u32> freqs_;   // 字符出现频率
    isize root_idx_ = NIL;         // 根索引

    void build_tree(std::priority_queue<Node>& pq) {
        if (pq.empty()) return;

        // 处理单字符特例
        if (pq.size() == 1) {
            Node dummy{nodes_.size(), 0, pq.top().idx, NIL};
            nodes_.append(dummy);
            root_idx_ = dummy.idx;
            return;
        }

        while (pq.size() > 1) {
            auto left = pq.top();
            pq.pop();
            auto right = pq.top();
            pq.pop();

            if (left.freq == right.freq && left.idx > right.idx) {
                std::swap(left, right);
            }

            Node parent(nodes_.size(), left.freq + right.freq, left.idx, right.idx);
            nodes_.append(parent);
            pq.push(parent);
        }
        root_idx_ = pq.top().idx;
    }

    void generate_key() {
        if (root_idx_ == NIL) return;
        traverse(root_idx_, ""_cs);
    }

    void traverse(isize cur, CString code) {
        if (cur == NIL) return;
        const Node& node = nodes_[cur];

        if (node.lch == NIL && node.rch == NIL) {
            key_[node.data] = code;
            return;
        }

        traverse(node.lch, code + "0"_cs);
        traverse(node.rch, code + "1"_cs);
    }

    isize height(isize cur_idx) const {
        if (cur_idx == NIL) return 0;
        const Node& node = nodes_[cur_idx];
        return 1 + std::max(height(node.lch), height(node.rch));
    }
};

} // namespace my::util

#endif // HUFFMAN_TREE_HPP