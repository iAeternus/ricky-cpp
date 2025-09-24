/**
 * @brief 有序字典，红黑树实现
 * @author Ricky
 * @date 2025/7/11
 * @version 1.0
 */
#ifndef SORTED_DICT_HPP
#define SORTED_DICT_HPP

#include "KeyValue.hpp"
#include "Color.hpp"
#include "Pair.hpp"

namespace my::util {

/**
 * @brief 红黑树节点约束
 */
template <typename T>
concept RBTreeNodeType = requires(T a, const T& b, T&& c) {
    typename T::key_t;
    typename T::value_t;
    { T() } -> std::same_as<T>;
    { a = std::move(c) } -> std::same_as<T&>;
    { T(std::move(c)) } -> std::same_as<T>;
    { a.key };
    { a.lch };
    { a.rch };
    { a.p };
};

/**
 * @brief 红黑树节点颜色
 */
enum class Color : bool {
    BLACK, // 黑 = 0
    RED,   // 红 = 1
};

/**
 * @class RBTreeNode
 * @brief 红黑树节点，存储键值对
 */
template <Sortable K, typename V>
struct RBTreeNode : Object<RBTreeNode<K, V>> {
    using key_t = K;
    using value_t = V;
    using Self = RBTreeNode<key_t, value_t>;
    using Callback = Consumer<const KeyValueView<key_t, value_t>&>;

    key_t key;   // 键
    value_t val; // 值
    Color color; // 颜色
    Self* lch;   // 指向左孩子的指针
    Self* rch;   // 指向右孩子的指针
    Self* p;     // 指向父节点的指针，定义根节点的父指针指向NIL

    explicit RBTreeNode(const key_t& key = key_t{}, const value_t& value = value_t{}, const Color color = Color::RED,
                        Self* lch = nullptr, Self* rch = nullptr, Self* parent = nullptr) :
            key(key), val(value), color(color), lch(lch), rch(rch), p(parent) {}

    explicit RBTreeNode(key_t&& key, value_t&& value, const Color color = Color::RED) :
            key(std::move(key)), val(std::move(value)), color(color), lch(nullptr), rch(nullptr), p(nullptr) {}

    RBTreeNode(const Self&) = delete;
    Self& operator=(const Self&) = delete;

    RBTreeNode(Self&& other) noexcept :
            key(std::move(other.key)),
            val(std::move(other.value)),
            color(other.color),
            lch(other.lch),
            rch(other.rch),
            p(other.p) {
        other.lch = other.rch = other.p = nullptr;
    }

    Self& operator=(Self&& other) noexcept {
        if (this == &other) return *this;

        this->color = other.color;
        this->key = std::move(other.key);
        this->val = std::move(other.val);
        this->lch = other.lch;
        this->rch = other.rch;
        this->p = other.p;
        other.lch = other.rch = other.p = nullptr;
        return *this;
    }

    /**
     * @brief 是否为黑节点
     * @return true=是 false=否
     */
    bool is_black() const noexcept {
        return color == Color::BLACK;
    }

    /**
     * @brief 是否为红节点
     * @return true=是 false=否
     */
    bool is_red() const noexcept {
        return color == Color::RED;
    }

    /**
     * @exception Exception 若键类型不可排序，则抛出 type_exception
     */
    [[nodiscard]] cmp_t __cmp__(const Self& other) const {
        if constexpr (Comparable<key_t>) {
            return this->key.__cmp__(other.key);
        } else if constexpr (Subtractble<key_t>) {
            return this->key - other.key;
        } else {
            throw type_exception("key type[{}] is not sortable", SRC_LOC, dtype(key_t));
        }
    }

    [[nodiscard]] CString __str__() const {
        std::stringstream stream;
        stream << (color == Color::RED ? io::Color::RED : "") << '(' << key << ',' << val << ')' << io::Color::CLOSE << '\n';
        return CString{stream.str()};
    }
};

/**
 * @class RBTree
 * @brief 红黑树
 * @note 满足如下红黑性质
 *       1. 每个节点要么是红色的，要么是黑色的
 *       2. 根节点是黑色的
 *       3. 叶结点（虚拟的外部节点，即NIL）是黑色的
 *       4. 不存在两个相邻的红节点（红节点的父节点和子节点均是黑色的）
 *       5. 对每个节点，从该节点到任意一个叶结点的简单路径上，所含黑节点的数量相同
 * @tparam Node 红黑树节点类型，须满足BiTreeNodeType约束
 * @tparam Comp 比较二元函数，定义第一个参数位于左子树
 * @tparam Alloc 内存分配器类型
 *
 */
template <RBTreeNodeType Node, typename Comp = std::less<typename Node::key_t>, typename Alloc = Allocator<Node>>
class RBTree : public Object<RBTree<Node, Comp, Alloc>> {
public:
    using Self = RBTree<Node, Comp, Alloc>;
    using key_t = typename Node::key_t;
    using value_t = typename Node::value_t;
    using Callback = std::function<void(const key_t&, const value_t&)>;

    /**
     * @brief 构造一棵空红黑树
     */
    explicit RBTree(Comp comp = Comp{}) :
            comp_(comp), size_(0), nil_(nullptr) {
        create_nil();
        this->root_ = nil_;
    }

    /**
     * @brief 通过初始化成员列表构造
     */
    RBTree(std::initializer_list<Pair<key_t, value_t>>&& init_list) :
            comp_(Comp{}), size_(0), nil_(nullptr) {
        create_nil();
        this->root_ = nil_;
        for (auto&& [key, val] : init_list) {
            insert(std::move(key), std::move(val));
        }
    }

    /**
     * @brief 拷贝构造
     */
    RBTree(const Self& other) :
            alloc_(other.alloc_),
            comp_(other.comp_),
            size_(other.size_),
            nil_(nullptr) {
        create_nil();
        this->root_ = nil_;
        other.for_each([&](const auto& key, const auto& val) {
            this->insert(key, val);
        });
    }

    /**
     * @brief 移动构造
     */
    RBTree(Self&& other) noexcept :
            alloc_(std::move(other.alloc_)),
            comp_(std::move(other.comp_)),
            size_(other.size_),
            root_(other.root_),
            nil_(other.nil_) {
        other.size_ = 0;
        other.root_ = other.nil_ = nullptr;
    }

    /**
     * @brief 拷贝赋值
     */
    Self& operator=(const Self& other) {
        if (this == &other) return *this;

        Self tmp(other);
        this->swap(tmp);
        return *this;
    }

    /**
     * @brief 移动赋值
     */
    Self& operator=(Self&& other) noexcept {
        if (this == &other) return *this;

        clear();
        if (nil_) {
            alloc_.destroy(nil_);
            alloc_.deallocate(nil_, 1);
        }

        this->alloc_ = std::move(other.alloc_);
        this->comp_ = std::move(other.comp_);
        this->size_ = other.size_;
        this->root_ = other.root_;
        this->nil_ = other.nil_;

        other.size_ = 0;
        other.root_ = other.nil_ = nullptr;
        return *this;
    }

    /**
     * @brief 析构
     */
    ~RBTree() {
        clear();
        if (nil_) {
            alloc_.destroy(nil_);
            alloc_.deallocate(nil_, 1);
            root_ = nil_ = nullptr;
        }
    }

    /**
     * @brief 获取节点个数
     */
    usize size() const noexcept {
        return size_;
    }

    /**
     * @brief 判断是否为空
     * @return true=是 false=否
     */
    bool empty() const noexcept {
        return root_ == nil_;
    }

    /**
     * @brief 获取最小的值
     * @exception Exception 若树空，则抛出 runtime_exception
     */
    value_t& front() {
        if (empty()) {
            throw runtime_exception("red-black-tree is empty");
        }
        return find_min(root_)->val;
    }

    /**
     * @brief 获取最小的值（常量版本）
     * @exception Exception 若树空，则抛出 runtime_exception
     */
    const value_t& front() const {
        if (empty()) {
            throw runtime_exception("red-black-tree is empty");
        }
        return find_min(root_)->val;
    }

    /**
     * @brief 获取最大的值
     * @exception Exception 若树空，则抛出 runtime_exception
     */
    value_t& back() {
        if (empty()) {
            throw runtime_exception("red-black-tree is empty");
        }
        return find_max(root_)->val;
    }

    /**
     * @brief 获取最大的值（常量版本）
     * @exception Exception 若树空，则抛出 runtime_exception
     */
    const value_t& back() const {
        if (empty()) {
            throw runtime_exception("red-black-tree is empty");
        }
        return find_max(root_)->val;
    }

    /**
     * @brief 检查字典中是否包含指定的键
     * @param key 需要检查的键
     * @return 如果键存在返回 true，否则返回 false
     */
    bool contains(const key_t& key) const {
        return tree_search(key) != nullptr;
    }

    /**
     * @brief 获取指定键对应的值
     * @param key 键
     * @return 返回对应值的可变引用
     * @exception Exception 若键不存在，则抛出 not_found_exception
     */
    template <typename K>
    value_t& get(const K& key) {
        Node* p = tree_search(key);
        if (p == nullptr) {
            throw not_found_exception("key '{}' not found in red-black-tree", SRC_LOC, key);
        }
        return p->val;
    }

    /**
     * @brief 获取指定键对应的值（常量版本）
     * @param key 键
     * @return 返回对应值的不可变引用
     * @exception Exception 若键不存在，则抛出 not_found_exception
     */
    template <typename K>
    const value_t& get(const K& key) const {
        Node* p = tree_search(key);
        if (p == nullptr) {
            throw not_found_exception("key '{}' not found in red-black-tree", SRC_LOC, key);
        }
        return p->val;
    }

    /**
     * @brief 获取指定键对应的值或默认值
     * @details 如果键不存在，返回默认值
     * @param key 键
     * @param default_val 默认值
     * @return 返回对应值的常量引用或默认值
     */
    template <typename K>
    const value_t& get_or_default(const K& key, const value_t& default_val) const {
        Node* p = tree_search(key);
        if (p == nullptr) {
            return default_val;
        }
        return p->val;
    }

    /**
     * @brief 获取指定键对应的值
     * @details 如果键不存在，创建键值对，值初始化为对应类型默认值
     * @param key 键
     * @return 返回对应值的可变引用
     */
    template <typename K>
    value_t& operator[](K&& key) {
        Node* p = tree_search(key);
        if (p == nullptr) {
            return insert(std::forward<K>(key), value_t{});
        }
        return p->val;
    }

    /**
     * @brief 如果键不存在，设置默认值，否则什么都不做
     * @param key 键
     * @param default_val 默认值
     * @return 本字典对象的引用，支持链式编程
     */
    template <typename V>
    Self& set_default(const key_t& key, V&& default_val) {
        Node* p = tree_search(key);
        if (p == nullptr) {
            insert(key, std::forward<V>(default_val));
        }
        return *this;
    }

    /**
     * @brief 构造并插入节点
     * @note 时间复杂度 O(log n)
     * @param args 构造节点的参数
     * @return 返回插入或更新后的值的引用
     */
    template <typename... Args>
    value_t& insert(Args&&... args) {
        auto& val = insert_impl(alloc_.create(std::forward<Args>(args)...));
        ++size_;
        return val;
    }

    /**
     * @brief 删除节点
     * @details 若节点不存在，则什么都不做
     * @param key 键
     */
    template <typename K>
    void remove(K&& key) {
        Node* z = tree_search(std::forward<K>(key));
        if (z != nullptr) {
            remove_impl(z);
            --size_;
            alloc_.destroy(z);
            alloc_.deallocate(z, 1);
        }
    }

    /**
     * @brief 清空所有节点
     */
    void clear() {
        clear(root_);
        root_ = nil_;
        size_ = 0;
    }

    /**
     * @brief 交换
     */
    void swap(Self& other) noexcept {
        std::swap(alloc_, other.alloc_);
        std::swap(comp_, other.comp_);
        std::swap(size_, other.size_);
        std::swap(root_, other.root_);
        std::swap(nil_, other.nil_);
    }

    /**
     * @brief 遍历所有键值对
     * @param callback 遍历时的回调函数
     */
    void for_each(Callback callback) const {
        inorder_for_each(root_, callback);
    }

    /**
     * @brief 逆序遍历所有键值对
     * @param callback 遍历时的回调函数
     */
    void for_each_rev(Callback callback) const {
        inorder_for_each_rev(root_, callback);
    }

    /**
     * @brief 计算两个字典的交集
     * @note 若两个字典的比较函数不同，则行为未定义
     * @param other 另一个字典
     * @return 返回两个字典的交集
     */
    Self operator&(const Self& other) const {
        Self res;
        auto it = this->begin();
        auto jt = other.begin();
        while (it != this->end() && jt != other.end()) {
            if (comp_(it->key(), jt->key())) {
                ++it;
            } else if (comp_(jt->key(), it->key())) {
                ++jt;
            } else {
                res.insert(it->key(), it->value());
                ++it;
                ++jt;
            }
        }
        return res;
    }

    /**
     * @brief 计算两个字典的交集并赋值给当前字典
     * @note 若两个字典的比较函数不同，则行为未定义
     * @param other 另一个字典
     * @return 本字典对象的引用
     */
    Self& operator&=(const Self& other) {
        if (this == &other) return *this;
        *this = *this & other;
        return *this;
    }

    /**
     * @brief 计算两个字典的并集，键相同则选择其他字典的值
     * @note 若两个字典的比较函数不同，则行为未定义
     * @param other 另一个字典
     * @return 返回两个字典的并集
     */
    Self operator|(const Self& other) const {
        Self res;
        auto it = this->begin();
        auto jt = other.begin();
        while (it != this->end() && jt != other.end()) {
            if (comp_(it->key(), jt->key())) {
                res.insert(it->key(), it->value());
                ++it;
            } else if (comp_(jt->key(), it->key())) {
                res.insert(jt->key(), jt->value());
                ++jt;
            } else {
                res.insert(it->key(), it->value());
                ++it;
                ++jt;
            }
        }
        while (it != this->end()) {
            res.insert(it->key(), it->value());
            ++it;
        }
        while (jt != other.end()) {
            res.insert(jt->key(), jt->value());
            ++jt;
        }
        return res;
    }

    /**
     * @brief 计算两个字典的并集并赋值给当前字典
     * @note 若两个字典的比较函数不同，则行为未定义
     * @param other 另一个字典
     * @return 本字典对象的引用
     */
    Self& operator|=(const Self& other) {
        if (this == &other) return *this;
        *this = *this | other;
        return *this;
    }

    /**
     * @brief 计算两个字典的并集
     * @note 若两个字典的比较函数不同，则行为未定义
     * @param other 另一个字典
     * @return 返回两个字典的并集
     */
    Self operator+(const Self& other) const {
        return *this | other;
    }

    /**
     * @brief 计算两个字典的并集并赋值给当前字典
     * @note 若两个字典的比较函数不同，则行为未定义
     * @param other 另一个字典
     * @return 本字典对象的引用
     */
    Self& operator+=(const Self& other) {
        return *this |= other;
    }

    /**
     * @brief 计算两个字典的相对补集
     * @note 若两个字典的比较函数不同，则行为未定义
     * @param other 另一个字典
     * @return 返回两个字典的相对补集
     */
    Self operator^(const Self& other) const {
        Self res;
        auto it = this->begin();
        auto jt = other.begin();
        while (it != this->end() && jt != other.end()) {
            if (comp_(it->key(), jt->key())) {
                res.insert(it->key(), it->value());
                ++it;
            } else if (comp_(jt->key(), it->key())) {
                res.insert(jt->key(), jt->value());
                ++jt;
            } else {
                ++it;
                ++jt;
            }
        }
        while (it != this->end()) {
            res.insert(it->key(), it->value());
            ++it;
        }
        while (jt != other.end()) {
            res.insert(jt->key(), jt->value());
            ++jt;
        }
        return res;
    }

    /**
     * @brief 计算两个字典的相对补集并赋值给当前字典
     * @note 若两个字典的比较函数不同，则行为未定义
     * @param other 另一个字典
     * @return 本字典对象的引用
     */
    Self& operator^=(const Self& other) {
        if (this == &other) return *this;
        *this = *this ^ other;
        return *this;
    }

    /**
     * @brief 计算两个字典的差集
     * @note 若两个字典的比较函数不同，则行为未定义
     * @param other 另一个字典
     * @return 返回两个字典的差集
     */
    Self operator-(const Self& other) const {
        Self res;
        auto it = this->begin();
        auto jt = other.begin();
        while (it != this->end() && jt != other.end()) {
            if (comp_(it->key(), jt->key())) {
                res.insert(it->key(), it->value());
                ++it;
            } else if (comp_(jt->key(), it->key())) {
                ++jt;
            } else {
                ++it;
                ++jt;
            }
        }
        while (it != this->end()) {
            res.insert(it->key(), it->value());
            ++it;
        }
        return res;
    }

    /**
     * @brief 计算两个字典的差集并赋值给当前字典
     * @note 若两个字典的比较函数不同，则行为未定义
     * @param other 另一个字典
     * @return 本字典对象的引用
     */
    Self& operator-=(const Self& other) {
        if (this == &other) return *this;
        *this = *this - other;
        return *this;
    }

    /**
     * @brief 获取树结构
     */
    [[nodiscard]] CString tree_struct() const {
        if (root_ == nil_) {
            return "(empty dict)"_cs;
        }
        std::stringstream stream;
        print_tree(root_, stream, "");
        return CString{stream.str()};
    }

    /**
     * @brief 比较两个字典的包含关系
     * @note 若两个字典的比较函数不同，则行为未定义
     * @note 建议使用__equals__比较集合相等
     * @return 返回值分以下情况
     * 1. 若this是other的真超集，返回 1
     * 2. 若this与other相等，返回 0
     * 3. 若this是other的真子集，返回 -1
     * 4. 若this与other无法比较（互不为子集），返回 TYPE_MAX(cmp_t)
     */
    [[nodiscard]] cmp_t __cmp__(const Self& other) const {
        auto it = this->begin();
        auto jt = other.begin();
        bool flag1 = false, flag2 = false;
        while (it != this->end() && jt != other.end()) {
            if (comp_(it->key(), jt->key())) {
                flag1 = true;
                ++it;
            } else if (comp_(jt->key(), it->key())) {
                flag2 = true;
                ++jt;
            } else {
                ++it;
                ++jt;
            }
        }
        if (it != this->end()) flag1 = true;
        if (jt != other.end()) flag2 = true;

        if (!flag1 && !flag2) {
            return 0; // 集合相等
        } else if (!flag2) {
            return 1; // this 是 other 的真超集
        } else if (!flag1) {
            return -1; // this 是 other 的真子集
        } else {
            return TYPE_MAX(cmp_t); // 互不为子集
        }
    }

    /**
     * @brief 比较两个字典是否相等
     * @note 若两个字典的比较函数不同，则行为未定义
     * @param other 另一个字典
     * @return 如果相等返回 true，否则返回 false
     */
    [[nodiscard]] bool __equals__(const Self& other) const {
        if (this->size() != other.size()) return false;

        auto it = this->begin();
        auto jt = other.begin();
        while (it != this->end() && jt != other.end()) {
            if (comp_(it->key(), jt->key())) {
                return false;
            } else if (comp_(jt->key(), it->key())) {
                return false;
            } else {
                ++it;
                ++jt;
            }
        }
        return true;
    }

    [[nodiscard]] CString __str__() const {
        std::stringstream stream;
        stream << '{';
        for_each([&](const auto& key, const auto& val) {
            if constexpr (is_same<key_t, CString /*, String*/, std::string>) {
                stream << '\"' << key << '\"';
            } else {
                stream << key;
            }
            stream << ':';
            if constexpr (is_same<value_t, CString /*, String*/, std::string>) {
                stream << '\"' << val << '\"';
            } else {
                stream << val;
            }
            stream << ',';
        });
        auto str = stream.str();
        if (str.size() > 1) {
            str.pop_back();
        }
        str.push_back('}');
        return CString{str};
    }

    /**
     * @brief 红黑树迭代器
     */
    class RBTreeIterator : public Object<RBTreeIterator> {
    public:
        using Self = RBTreeIterator;

        using iterator_category = std::bidirectional_iterator_tag;
        using value_type = KeyValueView<key_t, value_t>;
        using difference_type = std::ptrdiff_t;
        using pointer = value_type*;
        using const_pointer = const value_type*;
        using reference = value_type&;
        using const_reference = const value_type&;

        explicit RBTreeIterator(const RBTree* tree = nullptr, Node* curr = nullptr) :
                tree_(tree), curr_(curr) {
            if (curr_ != tree_->nil_ && curr_ != nullptr) {
                update_kv();
            }
        }

        RBTreeIterator(const Self& other) = default;
        Self& operator=(const Self& other) = default;

        /**
         * @brief 解引用
         */
        const_reference operator*() const { return kv_; }
        const_pointer operator->() const { return &kv_; }

        /**
         * @breif 前缀递增/递减
         */
        Self& operator++() {
            if (curr_ != tree_->nil_) {
                curr_ = tree_->next(curr_);
                update_kv();
            }
            return *this;
        }

        Self& operator--() {
            if (curr_ != tree_->nil_) {
                curr_ = tree_->prev(curr_);
                update_kv();
            }
            return *this;
        }

        /**
         * @breif 后缀递增/递减
         */
        Self operator++(int) {
            Self tmp = *this;
            ++*this;
            return tmp;
        }

        Self operator--(i32) {
            Self tmp = *this;
            --*this;
            return tmp;
        }

        [[nodiscard]] bool __equals__(const Self& other) const {
            return this->tree_ == other.tree_ && this->curr_ == other.curr_;
        }

    private:
        /**
         * @brief 更新当前键值对
         */
        void update_kv() {
            if (curr_ != tree_->nil_) {
                kv_.set(&curr_->key, &curr_->val);
            }
        }

    private:
        const RBTree* tree_; // 指向红黑树的指针
        Node* curr_;         // 迭代器当前位置
        value_type kv_;      // 当前键值对
    };

    using iterator = RBTreeIterator;
    using const_iterator = RBTreeIterator;

    /**
     * @brief 获取字典的起始迭代器
     * @return 返回起始迭代器
     */
    iterator begin() {
        if (!root_) {
            return iterator(nullptr, nil_);
        }
        return iterator(this, find_min(root_));
    }

    /**
     * @brief 获取字典的起始迭代器（常量版本）
     * @return 返回常量起始迭代器
     */
    const_iterator begin() const {
        if (!root_) {
            return const_iterator(nullptr, nil_);
        }
        return const_iterator(this, find_min(root_));
    }

    /**
     * @brief 获取字典的末尾迭代器
     * @return 返回末尾迭代器
     */
    iterator end() {
        return iterator(this, nil_);
    }

    /**
     * @brief 获取字典的末尾迭代器（常量版本）
     * @return 返回常量末尾迭代器
     */
    const_iterator end() const {
        return const_iterator(this, nil_);
    }

private:
    /**
     * @brief 创建哨兵节点
     */
    void create_nil() {
        nil_ = alloc_.allocate(1);
        alloc_.construct(nil_, key_t{}, value_t{}, Color::BLACK);
        nil_->lch = nil_->rch = nil_->p = nil_;
    }

    /**
     * @brief 清空所有节点，递归辅助函数
     */
    void clear(Node* root) {
        if (root == nil_) return;
        clear(root->lch);
        clear(root->rch);
        alloc_.destroy(root);
        alloc_.deallocate(root, 1);
    }

    /**
     * @brief 打印树结构，递归辅助函数
     */
    void print_tree(const Node* root, std::stringstream& stream, const CString& prefix) const {
        if (root == nil_) return;
        stream << prefix.data() << "+-- " << root->__str__().data();
        print_tree(root->lch, stream, prefix + "|   ");
        print_tree(root->rch, stream, prefix + "|   ");
    }

    /**
     * @brief 中序遍历
     */
    void inorder_for_each(Node* node, Callback callback) const {
        if (node == nil_) return;
        inorder_for_each(node->lch, callback);
        callback(node->key, node->val);
        inorder_for_each(node->rch, callback);
    }

    /**
     * @brief 逆中序遍历
     */
    void inorder_for_each_rev(Node* node, Callback callback) const {
        if (node == nil_) return;
        inorder_for_each_rev(node->rch, callback);
        callback(node->key, node->val);
        inorder_for_each_rev(node->lch, callback);
    }

    /**
     * @brief 插入节点z
     * @return 返回插入或更新后的值的引用
     */
    value_t& insert_impl(Node* z) {
        Node* y = nil_;
        Node* x = root_;
        while (x != nil_) {
            y = x;
            if (comp_(z->key, x->key)) {
                x = x->lch;
            } else {
                x = x->rch;
            }
        }
        z->p = y;
        if (y == nil_) {
            root_ = z;
        } else if (comp_(z->key, y->key)) {
            y->lch = z;
        } else {
            y->rch = z;
        }
        z->lch = z->rch = nil_;
        z->color = Color::RED;
        insert_fixup(z); // 调整
        return z->val;
    }

    /**
     * @brief 左旋
     *     |              |
     *     y    l_rot     x
     *    / \   <====    / \
     *   x   c          a   y
     *  / \     ====>      / \
     * a   b    r_rot     b   c
     */
    void left_rotate(Node* x) {
        Node* y = x->rch; // 设置y
        x->rch = y->lch;  // 将y的左子树变为x的右子树
        if (y->lch != nil_) {
            y->lch->p = x;
        }
        y->p = x->p;
        if (x->p == nil_) {
            root_ = y;
        } else if (x == x->p->lch) {
            x->p->lch = y;
        } else {
            x->p->rch = y;
        }
        y->lch = x; // 将x放在y的左边
        x->p = y;
    }

    /**
     * @brief 右旋
     *     |              |
     *     y    l_rot     x
     *    / \   <====    / \
     *   x   c          a   y
     *  / \     ====>      / \
     * a   b    r_rot     b   c
     */
    void right_rotate(Node* x) {
        Node* y = x->lch; // 设置y
        x->lch = y->rch;  // 将y的右子树变为x的左子树
        if (y->rch != nil_) {
            y->rch->p = x;
        }
        y->p = x->p;
        if (x->p == nil_) {
            root_ = y;
        } else if (x == x->p->rch) {
            x->p->rch = y;
        } else {
            x->p->lch = y;
        }
        y->rch = x; // 将x放在y的右边
        x->p = y;
    }

    /**
     * @brief 插入节点后调整
     * @note 存在以下3种情况
     * Case 1. z的叔节点y是红色的：将父/叔/爷节点变色，当前节点指向爷节点，继续调整
     * Case 2. z的叔节点y是黑色的且z是一个右孩子：左旋，转换为 case 3
     * Case 3. z的叔节点y是黑色的且z是一个左孩子：将父/爷节点变色，对爷节点右旋
     */
    void insert_fixup(Node* z) {
        while (z->p->is_red()) {
            if (z->p == z->p->p->lch) {
                Node* y = z->p->p->rch; // 叔节点
                if (y->is_red()) {
                    // Case 1: 叔节点为红色
                    z->p->color = y->color = Color::BLACK;
                    z->p->p->color = Color::RED;
                    z = z->p->p;
                } else {
                    // Case 2/3: 叔节点为黑色
                    if (z == z->p->rch) {
                        // Case 2: z是右孩子
                        z = z->p;
                        left_rotate(z);
                    }
                    // Case 3: z是左孩子
                    z->p->color = Color::BLACK;
                    z->p->p->color = Color::RED;
                    right_rotate(z->p->p);
                }
            } else {
                // 对称情况
                Node* y = z->p->p->lch;
                if (y->is_red()) {
                    z->p->color = Color::BLACK;
                    y->color = Color::BLACK;
                    z->p->p->color = Color::RED;
                    z = z->p->p;
                } else {
                    if (z == z->p->lch) {
                        z = z->p;
                        right_rotate(z);
                    }
                    z->p->color = Color::BLACK;
                    z->p->p->color = Color::RED;
                    left_rotate(z->p->p);
                }
            }
        }
        root_->color = Color::BLACK;
    }

    /**
     * @brief 获取以node为根的子树的最小元素
     */
    Node* find_min(Node* node) const {
        if (node == nil_) return nil_;
        while (node->lch != nil_) {
            node = node->lch;
        }
        return node;
    }

    /**
     * @brief 获取以node为根的子树的最大元素
     */
    Node* find_max(Node* node) const {
        if (node == nil_) return nil_;
        while (node->rch != nil_) {
            node = node->rch;
        }
        return node;
    }

    /**
     * @brief 树上查找
     * @param key 键
     * @return 若找到，返回指向目标节点的指针，否则返回 nullptr
     */
    template <typename K>
    Node* tree_search(const K& key) const {
        Node* p = root_;
        while (p != nil_) {
            if (comp_(key, p->key)) {
                p = p->lch;
            } else if (comp_(p->key, key)) {
                p = p->rch;
            } else {
                return p;
            }
        }
        return nullptr;
    }

    /**
     * @brief 用一棵以 v 为根的子树替换以 u 为根的子树，并成为后者父亲的孩子结点
     */
    void transplant(Node* u, Node* v) {
        if (u->p == nil_) {
            root_ = v;
        } else if (u == u->p->lch) {
            u->p->lch = v;
        } else {
            u->p->rch = v;
        }
        v->p = u->p;
    }

    /**
     * @brief 删除节点z
     */
    void remove_impl(Node* z) {
        Node *y = z, *x = nil_;
        Color y_original_color = y->color;
        if (z->lch == nil_) {
            x = z->rch;
            transplant(z, z->rch);
        } else if (z->rch == nil_) {
            x = z->lch;
            transplant(z, z->lch);
        } else {
            y = find_min(z->rch);
            y_original_color = y->color;
            x = y->rch;
            if (y->p == z) {
                x->p = y;
            } else {
                transplant(y, y->rch);
                y->rch = z->rch;
                y->rch->p = y;
            }
            transplant(z, y);
            y->lch = z->lch;
            y->lch->p = y;
            y->color = z->color;
        }
        if (y_original_color == Color::BLACK) {
            delete_fixup(x);
        }
    }

    /**
     * @brief 删除节点后调整
     * Case 1. 兄弟节点w是红色的：兄父变色，对父节点左旋，转换为其他情况
     * Case 2. 兄弟节点w是黑色，且w的两个子节点都是黑色的：兄弟变红，双黑上移
     * Case 3. 兄弟节点w是黑色，w的左孩子是红色的，右孩子是黑色的：交换兄弟与其左孩子的颜色，对兄弟右旋，转换为 Case 4
     * Case 4. 兄弟节点w是黑色，w的右孩子是红色的：变色，对父节点左旋
     */
    void delete_fixup(Node* x) {
        while (x != root_ && x->is_black()) {
            if (x == x->p->lch) {
                Node* w = x->p->rch; // 兄弟节点
                if (w->is_red()) {
                    // Case 1: 兄弟节点w是红色的
                    w->color = Color::BLACK;
                    x->p->color = Color::RED;
                    left_rotate(x->p);
                    w = x->p->rch;
                }
                if ((w->lch == nil_ || w->lch->is_black()) && (w->rch == nil_ || w->rch->is_black())) {
                    // Case 2: 兄弟节点w是黑色，且w的两个子节点都是黑色的
                    w->color = Color::RED;
                    x = x->p;
                } else {
                    if (w->rch->is_black()) {
                        // Case 3: 兄弟节点w是黑色，w的左孩子是红色，右孩子是黑色
                        w->lch->color = Color::BLACK;
                        w->color = Color::RED;
                        right_rotate(w);
                        w = x->p->rch;
                    }
                    // Case 4: 兄弟节点w是黑色，w的右孩子是红色
                    w->color = x->p->color;
                    x->p->color = Color::BLACK;
                    if (w->rch != nil_) {
                        w->rch->color = Color::BLACK;
                    }
                    left_rotate(x->p);
                    x = root_; // 终止循环
                }
            } else {
                // 对称情况
                Node* w = x->p->lch;
                if (w->is_red()) {
                    w->color = Color::BLACK;
                    x->p->color = Color::RED;
                    right_rotate(x->p);
                    w = x->p->lch;
                }
                if ((w->lch == nil_ || w->lch->is_black()) && (w->rch == nil_ || w->rch->is_black())) {
                    w->color = Color::RED;
                    x = x->p;
                } else {
                    if (w->lch->is_black()) {
                        w->rch->color = Color::BLACK;
                        w->color = Color::RED;
                        left_rotate(w);
                        w = x->p->lch;
                    }
                    w->color = x->p->color;
                    x->p->color = Color::BLACK;
                    if (w->lch != nil_) {
                        w->lch->color = Color::BLACK;
                    }
                    right_rotate(x->p);
                    x = root_;
                }
            }
        }
        x->color = Color::BLACK;
    }

    /**
     * @brief 获取 curr 的中序后继
     */
    Node* next(Node* curr) const {
        if (!curr) return nullptr;
        if (curr->rch != nil_) {
            return find_min(curr->rch);
        }
        // 右子树为空时，找到第一个有左孩子的祖先节点
        Node* next = curr->p;
        while (next != nil_ && curr == next->rch) {
            curr = next;
            next = next->p;
        }
        return next;
    }

    /**
     * @brief 获取 curr 的中序前驱
     */
    Node* prev(Node* curr) const {
        if (!curr) return nullptr;
        if (curr->lch != nil_) {
            return find_max(curr->lch);
        }
        // 左子树为空时，找到第一个有右孩子的祖先节点
        Node* prev = curr->p;
        while (prev != nil_ && curr == prev->lch) {
            curr = prev;
            prev = prev->p;
        }
        return prev;
    }

private:
    Alloc alloc_{}; // 内存分配器
    Comp comp_;     // 比较函数
    usize size_;    // 节点个数
    Node* root_;    // 指向根节点的指针
    Node* nil_;     // 哨兵：所有叶结点的孩子且是根节点的父亲
};

/**
 * @brief 有序字典
 */
template <Sortable K, typename V, typename Comp = std::less<K>, typename Alloc = Allocator<RBTreeNode<K, V>>>
using SortedDict = RBTree<RBTreeNode<K, V>, Comp, Alloc>;

} // namespace my::util

#endif // SORTED_DICT_HPP
