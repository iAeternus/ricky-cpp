/**
 * @brief 哈夫曼树
 * @author Ricky
 * @date 2025/5/11
 * @version 1.0
 */
#ifndef HUFFMAN_TREE_HPP
#define HUFFMAN_TREE_HPP

#include "StringBuilder.hpp"
#include "Dict.hpp"
#include "PriorityQueue.hpp"
#include "NoCopy.hpp"

namespace my::util {

/**
 * @class HuffmanTree
 * @brief 哈夫曼树类，用于文本编码和解码
 *
 * HuffmanTree 是一个基于哈夫曼编码算法的树结构，
 * 用于对文本进行压缩编码和解码，支持计算带权路径长度和平均编码长度
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
        CodePoint data;
        u32 freq;
        usize lch = NIL, rch = NIL;

        Node(usize idx, const CodePoint& data, u32 freq) :
                idx(idx), data(data), freq(freq) {}

        Node(usize idx, u32 freq, usize lch, usize rch) :
                idx(idx), freq(freq), lch(lch), rch(rch) {}

        bool operator<(const Node& other) const {
            return this->freq < other.freq;
        }
    };

    /**
     * @brief 哈夫曼树构造函数
     * @param text 输入的文本，用于统计字符频率
     */
    HuffmanTree(const String& text) :
            text_(text) {
        for (const auto& cp : text) {
            ++freqs_[cp];
        }

        usize idx = 0;
        PriorityQueue<Node> pq;
        for (const auto& [cp, freq] : freqs_) {
            nodes_.append(idx++, cp, freq);
            pq.push(nodes_.back());
        }

        build_tree(pq);
        generate_key();
    }

    /**
     * @brief 编码文本
     * @return 返回编码后的字符串
     * @throws runtime_exception 如果编码过程中缺少字符对应的编码
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
     * @throws runtime_exception 如果编码流无效或缺少字符对应的编码
     */
    String decode() {
        StringBuilder sb;
        usize cur = root_idx_;
        for (char bit : encoded_text_) {
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
    fn get_key() const {
        return key_;
    }

    /**
     * @brief 获取字符频率
     */
    fn get_freqs() const {
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
        return wpl() / f64(text_.length());
    }

    /**
     * @brief 计算树高
     */
    usize height() const {
        if (root_idx_ == NIL) throw runtime_exception("please build the tree first.");
        return height(root_idx_);
    }

private:
    /**
     * @brief 构建哈夫曼树
     * @param pq 优先队列，包含所有节点
     * @note 使用优先队列构建哈夫曼树，合并频率最小的两个节点直到只剩一个根节点
     */
    void build_tree(PriorityQueue<Node>& pq) {
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

    void traverse(usize cur, CString code) {
        if (cur == NIL) return;
        const Node& node = nodes_[cur];

        if (node.lch == NIL && node.rch == NIL) {
            key_[node.data] = code;
            return;
        }

        traverse(node.lch, code + "0"_cs);
        traverse(node.rch, code + "1"_cs);
    }

    usize height(usize cur_idx) const {
        if (cur_idx == NIL) return 0;
        const Node& node = nodes_[cur_idx];
        return 1 + std::max(height(node.lch), height(node.rch));
    }

private:
    String text_;                  // 原始文本
    String encoded_text_;          // 编码后的文本
    Vec<Node> nodes_;              // 树结构
    Dict<CodePoint, CString> key_; // 密钥
    Dict<CodePoint, u32> freqs_;   // 字符出现频率
    usize root_idx_ = NIL;         // 根索引
};

} // namespace my::util

#endif // HUFFMAN_TREE_HPP