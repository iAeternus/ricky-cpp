/**
 * @brief 哈夫曼树
 * @author Ricky
 * @date 2025/5/11
 * @version 1.0
 */
#ifndef HUFFMAN_TREE_HPP
#define HUFFMAN_TREE_HPP

#include "str_builder.hpp"
#include "hash_map.hpp"
#include "binary_heap.hpp"
#include "marker.hpp"

namespace my::util {

/**
 * @class HuffmanTree
 * @brief 哈夫曼树类，用于文本编码和解码
 * @details 用于对文本进行压缩编码和解码，支持计算带权路径长度和平均编码长度
 */
class HuffmanTree : public Object<HuffmanTree>, public NoCopyMove {
public:
    using Self = HuffmanTree;
    static constexpr usize NIL = -1ULL;

    /**
     * @brief 哈夫曼树节点
     * @note 节点包含索引、字符数据、频率以及左右子节点索引
     */
    struct Node {
        usize idx;
        CodePoint<> data;
        u32 freq;
        usize lch = NIL, rch = NIL;

        Node(const usize idx, const CodePoint<>& data, const u32 freq) :
                idx(idx), data(data), freq(freq) {}

        Node(const usize idx, const u32 freq, const usize lch, const usize rch) :
                idx(idx), freq(freq), lch(lch), rch(rch) {}

        bool operator<(const Node& other) const {
            return this->freq < other.freq;
        }
    };

    /**
     * @brief 哈夫曼树构造函数
     * @param text 输入的文本，用于统计字符频率
     */
    explicit HuffmanTree(const String& text) :
            text_(text) {
        for (const auto& cp : text) {
            ++freqs_[cp];
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

    /**
     * @brief 编码文本
     * @return 返回编码后的字符串
     * @exception Exception 若编码过程中缺少字符对应的编码，则抛出 runtime_exception
     */
    String encode() {
        StringBuilder sb;
        for (const auto& cp : text_) {
            if (!key_.contains(cp)) {
                throw runtime_exception("missing code for character");
            }
            sb.append(key_[cp].data());
        }
        encoded_text_ = sb.build();
        return encoded_text_;
    }

    /**
     * @brief 解码文本
     * @return 返回解码后的字符串
     * @exception Exception 若编码流无效或缺少字符对应的编码，则抛出 runtime_exception
     */
    String decode() {
        StringBuilder sb;
        usize cur = root_idx_;
        for (const char bit : encoded_text_) {
            cur = (bit == '0') ? nodes_[cur].lch : nodes_[cur].rch;
            if (cur == NIL) throw runtime_exception("invalid encoded stream");

            if (nodes_[cur].lch == NIL && nodes_[cur].rch == NIL) {
                sb.append(nodes_[cur].data);
                cur = root_idx_;
            }
        }
        return sb.build();
    }

    /**
     * @brief 获取密钥
     */
    auto get_key() const {
        return key_;
    }

    /**
     * @brief 获取字符频率
     */
    auto get_freqs() const {
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
     * @brief 计算平均编码长度，别名平均带权路径长度
     * @note Average Code Length/Average Weighted Path Length
     */
    f64 acl() const {
        if (freqs_.empty()) return 0.0;
        return wpl() / static_cast<f64>(text_.len());
    }

    /**
     * @brief 计算树高
     * @exception Exception 若未构建哈夫曼树就计算树高，则抛出 runtime_exception
     */
    usize height() const {
        if (root_idx_ == NIL) throw runtime_exception("please build the tree first.");
        return height(root_idx_);
    }

private:
    /**
     * @brief 构建哈夫曼树
     * @param bh 优先队列，包含所有节点
     * @note 使用优先队列构建哈夫曼树，合并频率最小的两个节点直到只剩一个根节点
     */
    void build_tree(BinaryHeap<Node>& bh) {
        if (bh.is_empty()) return;

        // 处理单字符特例
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
        traverse(root_idx_, ""_cs);
    }

    void traverse(const usize cur, const CString& code) {
        if (cur == NIL) return;
        const Node& node = nodes_[cur];

        if (node.lch == NIL && node.rch == NIL) {
            key_[node.data] = code;
            return;
        }

        traverse(node.lch, code + "0"_cs);
        traverse(node.rch, code + "1"_cs);
    }

    usize height(const usize cur_idx) const {
        if (cur_idx == NIL) return 0;
        const Node& node = nodes_[cur_idx];
        return 1 + std::max(height(node.lch), height(node.rch));
    }

private:
    String text_;                       // 原始文本
    String encoded_text_;               // 编码后的文本
    Vec<Node> nodes_;                   // 树结构
    HashMap<CodePoint<>, CString> key_; // 密钥
    HashMap<CodePoint<>, u32> freqs_;   // 字符出现频率
    usize root_idx_ = NIL;              // 根索引
};

} // namespace my::util

#endif // HUFFMAN_TREE_HPP