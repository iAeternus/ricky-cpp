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
    { T() }
    ->std::same_as<T>;
    { a = std::move(c) }
    ->std::same_as<T&>;
    { T(std::move(c)) }
    ->std::same_as<T>;
    {a.key_};
    {a.lch_};
    {a.rch_};
    {a.p_};
};

/**
 * @brief 红黑树节点颜色
 */
enum class Color : bool {
    BLACK, // 黑 = 0
    RED,   // 红 = 1
};

/**
 * @brief 红黑树节点，存储键值对
 */
template <Sortable K, typename V>
class RBTreeNode : public Object<RBTreeNode<K, V>> {
public:
    using key_t = K;
    using value_t = V;
    using Self = RBTreeNode<key_t, value_t>;
    using Callback = Consumer<const KeyValueView<key_t, value_t>&>;

    key_t key_;   // 键
    value_t val_; // 值
    Color color_; // 颜色
    Self* lch_;   // 指向左孩子的指针
    Self* rch_;   // 指向右孩子的指针
    Self* p_;     // 指向父节点的指针，定义根节点的父指针指向NIL

    explicit RBTreeNode(const key_t& key = key_t{}, const value_t& value = value_t{}, Color color = Color::RED,
                        Self* lchild = nullptr, Self* rchild = nullptr, Self* parent = nullptr) :
            key_(key), val_(value), color_(color), lch_(lchild), rch_(rchild), p_(parent) {}

    explicit RBTreeNode(key_t&& key, value_t&& value, Color color = Color::RED) :
            key_(std::move(key)), val_(std::move(value)), color_(color), lch_(nullptr), rch_(nullptr), p_(nullptr) {}

    RBTreeNode(const Self&) = delete;
    Self& operator=(const Self&) = delete;

    RBTreeNode(Self&& other) noexcept :
            color_(other.color_),
            key_(std::move(other.key_)),
            val_(std::move(other.value)),
            lch_(other.lch_),
            rch_(other.rch_),
            p_(other.p_) {
        other.lch_ = other.rch_ = other.p_ = nullptr;
    }

    Self& operator=(Self&& other) noexcept {
        if (this == &other) return *this;

        this->color_ = other.color_;
        this->key_ = std::move(other.key_);
        this->val_ = std::move(other.val_);
        this->lch_ = other.lch_;
        this->rch_ = other.rch_;
        this->p_ = other.p_;
        other.lch_ = other.rch_ = other.p_ = nullptr;
        return *this;
    }

    /**
     * @brief 是否为黑节点
     * @return true=是 false=否
     */
    bool is_black() const noexcept {
        return color_ == Color::BLACK;
    }

    /**
     * @brief 是否为红节点
     * @return true=是 false=否
     */
    bool is_red() const noexcept {
        return color_ == Color::RED;
    }

    [[nodiscard]] cmp_t __cmp__(const Self& other) const {
        if constexpr (Comparable<key_t>) {
            return this->key_.__cmp__(other.key_);
        } else if constexpr (Subtractble<key_t>) {
            return this->key_ - other.key_;
        } else {
            throw type_exception("key type[{}] is not sortable", SRC_LOC, dtype(key_t));
        }
    }

    [[nodiscard]] CString __str__() const {
        std::stringstream stream;
        stream << (color_ == Color::RED ? io::Color::RED : "") << '(' << key_ << ',' << val_ << ')' << io::Color::CLOSE << '\n';
        return CString{stream.str()};
    }
};

/**
 * @brief 红黑树
 * @tparam Node 红黑树节点类型，须满足BiTreeNodeType约束
 * @tparam Comp 比较二元函数，定义第一个参数位于左子树
 * @tparam Alloc 内存分配器类型
 * @note 满足如下红黑性质
 * 1. 每个节点要么是红色的，要么是黑色的
 * 2. 根节点是黑色的
 * 3. 叶结点（虚拟的外部节点，即NIL）是黑色的
 * 4. 不存在两个相邻的红节点（红节点的父节点和子节点均是黑色的）
 * 5. 对每个节点，从该节点到任意一个叶结点的简单路径上，所含黑节点的数量相同
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
    RBTree(Comp comp = Comp{}) :
            comp_(comp), size_(0) {
        create_nil();
        this->root_ = nil_;
    }

    /**
     * @brief 通过初始化成员列表构造
     */
    RBTree(std::initializer_list<Pair<key_t, value_t>>&& init_list) :
            comp_(Comp{}), size_(0) {
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
            size_(other.size_) {
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
     * @brief 获取最小的值，若树空抛出 runtime_exception
     */
    value_t& front() {
        if (empty()) {
            throw runtime_exception("red-black-tree is empty");
        }
        return find_min(root_)->val_;
    }

    /**
     * @brief 获取最小的值，若树空抛出 runtime_exception（常量版本）
     */
    const value_t& front() const {
        if (empty()) {
            throw runtime_exception("red-black-tree is empty");
        }
        return find_min(root_)->val_;
    }

    /**
     * @brief 获取最大的值，若树空抛出 runtime_exception
     */
    value_t& back() {
        if (empty()) {
            throw runtime_exception("red-black-tree is empty");
        }
        return find_max(root_)->val_;
    }

    /**
     * @brief 获取最大的值，若树空抛出 runtime_exception（常量版本）
     */
    const value_t& back() const {
        if (empty()) {
            throw runtime_exception("red-black-tree is empty");
        }
        return find_max(root_)->val_;
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
     * 如果键不存在，抛出 throw not_found_exception
     * @param key 键
     * @return 返回对应值的可变引用
     */
    value_t& get(const key_t& key) {
        Node* p = tree_search(key);
        if (p == nullptr) {
            throw not_found_exception("key '{}' not found in red-black-tree", SRC_LOC, key);
        }
        return p->val_;
    }

    /**
     * @brief 获取指定键对应的值（常量版本）
     * 如果键不存在，抛出 throw not_found_exception
     * @param key 键
     * @return 返回对应值的不可变引用
     */
    const value_t& get(const key_t& key) const {
        Node* p = tree_search(key);
        if (p == nullptr) {
            throw not_found_exception("key '{}' not found in red-black-tree", SRC_LOC, key);
        }
        return p->val_;
    }

    /**
     * @brief 获取指定键对应的值，默认值
     * 如果键不存在，返回默认值
     * @param key 键
     * @param default_val 默认值
     * @return 返回对应值的引用或默认值
     */
    value_t& get_or_default(const key_t& key, value_t& default_val) {
        Node* p = tree_search(key);
        if (p == nullptr) {
            return default_val;
        }
        return p->val_;
    }

    /**
     * @brief 获取指定键对应的值，默认值（常量版本）
     * 如果键不存在，返回默认值
     * @param key 键
     * @param default_val 默认值
     * @return 返回对应值的常量引用或默认值
     */
    const value_t& get_or_default(const key_t& key, const value_t& default_val) const {
        Node* p = tree_search(key);
        if (p == nullptr) {
            return default_val;
        }
        return p->val_;
    }

    /**
     * @brief 获取指定键对应的值
     * 如果键不存在，创建键值对，值初始化为对应类型默认值
     * @param key 键
     * @return 返回对应值的可变引用
     */
    value_t& operator[](const key_t& key) {
        Node* p = tree_search(key);
        if (p == nullptr) {
            return insert(key, value_t{});
        }
        return p->val_;
    }

    /**
     * @brief 如果键不存在，设置默认值，否则什么都不做
     * @param key 键
     * @param default_val 默认值
     * @return 本字典对象的引用，支持链式编程
     */
    template <typename _V>
    Self& set_default(const key_t& key, _V&& default_val) {
        Node* p = tree_search(key);
        if (p == nullptr) {
            insert(key, std::forward<_V>(default_val));
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
     * 若节点不存在，则什么都不做
     * @param key 键
     */
    template <typename _K>
    void remove(_K&& key) {
        Node* z = tree_search(std::forward<_K>(key));
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
        } else if (flag1 && !flag2) {
            return 1; // this 是 other 的真超集
        } else if (!flag1 && flag2) {
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
            if constexpr (is_same<key_t, CString, String, std::string>) {
                stream << '\"' << key << '\"';
            } else {
                stream << key;
            }
            stream << ':';
            if constexpr (is_same<value_t, CString, String, std::string>) {
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

        RBTreeIterator(const RBTree* tree = nullptr, Node* curr = nullptr) :
                tree_(tree), curr_(curr) {
            if (curr_ != tree_->nil_ && curr_ != nullptr) {
                update_kv();
            }
        }

        RBTreeIterator(const Self& other) = default;
        Self& operator=(const Self& other) = default;

        /**
         * @brief 解引用运算符
         * @return 返回当前键值对的引用
         */
        const_reference operator*() const {
            return kv_;
        }

        /**
         * @brief 获取指向当前键值对的指针
         * @return 返回指向当前键值对的指针
         */
        const_pointer operator->() const {
            return &kv_;
        }

        /**
         * @brief 前置自增运算符
         * 移动迭代器到下一个键值对
         * @return 返回自增后的迭代器
         */
        Self& operator++() {
            if (curr_ != tree_->nil_) {
                curr_ = tree_->next(curr_);
                update_kv();
            }
            return *this;
        }

        /**
         * @brief 后置自增运算符
         * 移动迭代器到下一个键值对
         * @return 返回自增前的迭代器
         */
        Self operator++(int) {
            Self tmp = *this;
            ++*this;
            return tmp;
        }

        /**
         * @brief 前置自减运算符
         * 移动迭代器到上一个键值对
         * @return 返回自减后的迭代器
         */
        Self& operator--() {
            if (curr_ != tree_->nil_) {
                curr_ = tree_->prev(curr_);
                update_kv();
            }
            return *this;
        }

        /**
         * @brief 后置自减运算符
         * 移动迭代器到上一个键值对
         * @return 返回自减前的迭代器
         */
        Self operator--(i32) {
            Self tmp = *this;
            --*this;
            return tmp;
        }

        /**
         * @brief 比较两个迭代器是否相等
         * @param other 另一个迭代器
         * @return 如果相等返回 true，否则返回 false
         */
        [[nodiscard]] bool __equals__(const Self& other) const {
            return this->tree_ == other.tree_ && this->curr_ == other.curr_;
        }

        /**
         * @brief 比较两个迭代器是否相等
         * @param other 另一个迭代器
         * @return 如果相等返回 true，否则返回 false
         */
        bool operator==(const Self& other) const {
            return this->__equals__(other);
        }

        /**
         * @brief 比较两个迭代器是否不相等
         * @param other 另一个迭代器
         * @return 如果不相等返回 true，否则返回 false
         */
        bool operator!=(const Self& other) const {
            return !this->__equals__(other);
        }

    private:
        /**
         * @brief 更新当前键值对
         * @param add 需要调整的指数
         */
        void update_kv() {
            if (curr_ != tree_->nil_) {
                kv_.set(&curr_->key_, &curr_->val_);
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
        nil_->lch_ = nil_->rch_ = nil_->p_ = nil_;
    }

    /**
     * @brief 清空所有节点，递归辅助函数
     */
    void clear(Node* root) {
        if (root == nil_) return;
        clear(root->lch_);
        clear(root->rch_);
        alloc_.destroy(root);
        alloc_.deallocate(root, 1);
    }

    /**
     * @brief 打印树结构，递归辅助函数
     */
    void print_tree(const Node* root, std::stringstream& stream, const CString& prefix) const {
        if (root == nil_) return;
        stream << prefix.data() << "+-- " << root->__str__().data();
        print_tree(root->lch_, stream, prefix + "|   ");
        print_tree(root->rch_, stream, prefix + "|   ");
    }

    /**
     * @brief 中序遍历
     */
    void inorder_for_each(Node* node, Callback callback) const {
        if (node == nil_) return;
        inorder_for_each(node->lch_, callback);
        callback(node->key_, node->val_);
        inorder_for_each(node->rch_, callback);
    }

    /**
     * @brief 逆中序遍历
     */
    void inorder_for_each_rev(Node* node, Callback callback) const {
        if (node == nil_) return;
        inorder_for_each_rev(node->rch_, callback);
        callback(node->key_, node->val_);
        inorder_for_each_rev(node->lch_, callback);
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
            if (comp_(z->key_, x->key_)) {
                x = x->lch_;
            } else {
                x = x->rch_;
            }
        }
        z->p_ = y;
        if (y == nil_) {
            root_ = z;
        } else if (comp_(z->key_, y->key_)) {
            y->lch_ = z;
        } else {
            y->rch_ = z;
        }
        z->lch_ = z->rch_ = nil_;
        z->color_ = Color::RED;
        insert_fixup(z); // 调整
        return z->val_;
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
        Node* y = x->rch_; // 设置y
        x->rch_ = y->lch_; // 将y的左子树变为x的右子树
        if (y->lch_ != nil_) {
            y->lch_->p_ = x;
        }
        y->p_ = x->p_;
        if (x->p_ == nil_) {
            root_ = y;
        } else if (x == x->p_->lch_) {
            x->p_->lch_ = y;
        } else {
            x->p_->rch_ = y;
        }
        y->lch_ = x; // 将x放在y的左边
        x->p_ = y;
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
        Node* y = x->lch_; // 设置y
        x->lch_ = y->rch_; // 将y的右子树变为x的左子树
        if (y->rch_ != nil_) {
            y->rch_->p_ = x;
        }
        y->p_ = x->p_;
        if (x->p_ == nil_) {
            root_ = y;
        } else if (x == x->p_->rch_) {
            x->p_->rch_ = y;
        } else {
            x->p_->lch_ = y;
        }
        y->rch_ = x; // 将x放在y的右边
        x->p_ = y;
    }

    /**
     * @brief 插入节点后调整
     * @note 存在以下3种情况
     * Case 1. z的叔节点y是红色的：将父/叔/爷节点变色，当前节点指向爷节点，继续调整
     * Case 2. z的叔节点y是黑色的且z是一个右孩子：左旋，转换为 case 3
     * Case 3. z的叔节点y是黑色的且z是一个左孩子：将父/爷节点变色，对爷节点右旋
     */
    void insert_fixup(Node* z) {
        while (z->p_->is_red()) {
            if (z->p_ == z->p_->p_->lch_) {
                Node* y = z->p_->p_->rch_; // 叔节点
                if (y->is_red()) {
                    // Case 1: 叔节点为红色
                    z->p_->color_ = y->color_ = Color::BLACK;
                    z->p_->p_->color_ = Color::RED;
                    z = z->p_->p_;
                } else {
                    // Case 2/3: 叔节点为黑色
                    if (z == z->p_->rch_) {
                        // Case 2: z是右孩子
                        z = z->p_;
                        left_rotate(z);
                    }
                    // Case 3: z是左孩子
                    z->p_->color_ = Color::BLACK;
                    z->p_->p_->color_ = Color::RED;
                    right_rotate(z->p_->p_);
                }
            } else {
                // 对称情况
                Node* y = z->p_->p_->lch_;
                if (y->is_red()) {
                    z->p_->color_ = Color::BLACK;
                    y->color_ = Color::BLACK;
                    z->p_->p_->color_ = Color::RED;
                    z = z->p_->p_;
                } else {
                    if (z == z->p_->lch_) {
                        z = z->p_;
                        right_rotate(z);
                    }
                    z->p_->color_ = Color::BLACK;
                    z->p_->p_->color_ = Color::RED;
                    left_rotate(z->p_->p_);
                }
            }
        }
        root_->color_ = Color::BLACK;
    }

    /**
     * @brief 获取以node为根的子树的最小元素
     */
    Node* find_min(Node* node) const {
        if (node == nil_) return nil_;
        while (node->lch_ != nil_) {
            node = node->lch_;
        }
        return node;
    }

    /**
     * @brief 获取以node为根的子树的最大元素
     */
    Node* find_max(Node* node) const {
        if (node == nil_) return nil_;
        while (node->rch_ != nil_) {
            node = node->rch_;
        }
        return node;
    }

    /**
     * @brief 树上查找
     * @param key 键
     * @return 若找到，返回指向目标节点的指针，否则返回 nullptr
     */
    template <typename K>
    Node* tree_search(K&& key) const {
        Node* p = root_;
        while (p != nil_) {
            if (comp_(key, p->key_)) {
                p = p->lch_;
            } else if (comp_(p->key_, key)) {
                p = p->rch_;
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
        if (u->p_ == nil_) {
            root_ = v;
        } else if (u == u->p_->lch_) {
            u->p_->lch_ = v;
        } else {
            u->p_->rch_ = v;
        }
        v->p_ = u->p_;
    }

    /**
     * @brief 删除节点z
     */
    void remove_impl(Node* z) {
        Node *y = z, *x = nil_;
        Color y_original_color = y->color_;
        if (z->lch_ == nil_) {
            x = z->rch_;
            transplant(z, z->rch_);
        } else if (z->rch_ == nil_) {
            x = z->lch_;
            transplant(z, z->lch_);
        } else {
            y = find_min(z->rch_);
            y_original_color = y->color_;
            x = y->rch_;
            if (y->p_ == z) {
                x->p_ = y;
            } else {
                transplant(y, y->rch_);
                y->rch_ = z->rch_;
                y->rch_->p_ = y;
            }
            transplant(z, y);
            y->lch_ = z->lch_;
            y->lch_->p_ = y;
            y->color_ = z->color_;
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
            if (x == x->p_->lch_) {
                Node* w = x->p_->rch_; // 兄弟节点
                if (w->is_red()) {
                    // Case 1: 兄弟节点w是红色的
                    w->color_ = Color::BLACK;
                    x->p_->color_ = Color::RED;
                    left_rotate(x->p_);
                    w = x->p_->rch_;
                }
                if ((w->lch_ == nil_ || w->lch_->is_black()) && (w->rch_ == nil_ || w->rch_->is_black())) {
                    // Case 2: 兄弟节点w是黑色，且w的两个子节点都是黑色的
                    w->color_ = Color::RED;
                    x = x->p_;
                } else {
                    if (w->rch_->is_black()) {
                        // Case 3: 兄弟节点w是黑色，w的左孩子是红色，右孩子是黑色
                        w->lch_->color_ = Color::BLACK;
                        w->color_ = Color::RED;
                        right_rotate(w);
                        w = x->p_->rch_;
                    }
                    // Case 4: 兄弟节点w是黑色，w的右孩子是红色
                    w->color_ = x->p_->color_;
                    x->p_->color_ = Color::BLACK;
                    if (w->rch_ != nil_) {
                        w->rch_->color_ = Color::BLACK;
                    }
                    left_rotate(x->p_);
                    x = root_; // 终止循环
                }
            } else {
                // 对称情况
                Node* w = x->p_->lch_;
                if (w->is_red()) {
                    w->color_ = Color::BLACK;
                    x->p_->color_ = Color::RED;
                    right_rotate(x->p_);
                    w = x->p_->lch_;
                }
                if ((w->lch_ == nil_ || w->lch_->is_black()) && (w->rch_ == nil_ || w->rch_->is_black())) {
                    w->color_ = Color::RED;
                    x = x->p_;
                } else {
                    if (w->lch_->is_black()) {
                        w->rch_->color_ = Color::BLACK;
                        w->color_ = Color::RED;
                        left_rotate(w);
                        w = x->p_->lch_;
                    }
                    w->color_ = x->p_->color_;
                    x->p_->color_ = Color::BLACK;
                    if (w->lch_ != nil_) {
                        w->lch_->color_ = Color::BLACK;
                    }
                    right_rotate(x->p_);
                    x = root_;
                }
            }
        }
        x->color_ = Color::BLACK;
    }

    /**
     * @brief 获取 curr 的中序后继
     */
    Node* next(Node* curr) const {
        if (!curr) return nullptr;
        if (curr->rch_ != nil_) {
            return find_min(curr->rch_);
        }
        // 右子树为空时，找到第一个有左孩子的祖先节点
        Node* next = curr->p_;
        while (next != nil_ && curr == next->rch_) {
            curr = next;
            next = next->p_;
        }
        return next;
    }

    /**
     * @brief 获取 curr 的中序前驱
     */
    Node* prev(Node* curr) const {
        if (!curr) return nullptr;
        if (curr->lch_ != nil_) {
            return find_max(curr->lch_);
        }
        // 左子树为空时，找到第一个有右孩子的祖先节点
        Node* prev = curr->p_;
        while (prev != nil_ && curr == prev->lch_) {
            curr = prev;
            prev = prev->p_;
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
template <Sortable K, typename V, typename Comp = std::less<K>>
using SortedDict = RBTree<RBTreeNode<K, V>, Comp, Allocator<RBTreeNode<K, V>>>;

} // namespace my::util

#endif // SORTED_DICT_HPP
