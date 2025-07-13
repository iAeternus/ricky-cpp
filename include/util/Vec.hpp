/**
 * @brief 向量
 * @author Ricky
 * @date 2025/2/28
 * @version 1.0
 */
#ifndef VEC_HPP
#define VEC_HPP

#include "Array.hpp"

#include <any>

namespace my::util {

/**
 * @brief 动态数组（向量）容器
 * @tparam T 元素类型
 * 
 * 该容器支持动态扩容，提供随机访问迭代器，时间复杂度如下：
 * - 随机访问：O(1)
 * - 末尾插入/删除：均摊 O(1)
 * - 中间插入/删除：O(n)
 */
template <typename T, typename Alloc = Allocator<T>>
class Vec : public Sequence<Vec<T>, T> {
public:
    using value_t = T;
    using Self = Vec<value_t, Alloc>;
    using Super = Sequence<Vec<value_t>, value_t>;

    /**
     * @brief 默认构造函数
     * @note 创建一个空向量，容量为0
     */
    Vec() :
            size_(0), capacity_(DEFAULT_CAPACITY), data_(alloc_.allocate(capacity_)) {}

    /**
     * @brief 构造指定大小的向量并用默认值填充
     * @param size 初始元素个数
     * @param val 用于填充的值，默认为值初始化
     */
    Vec(usize size, const value_t& val = value_t{}) :
            size_(size), capacity_(size_), data_(alloc_.allocate(capacity_)) {
        for (usize i = 0; i < size_; ++i) {
            alloc_.construct(data_ + i, val);
        }
    }

    /**
     * @brief 从初始化列表构造
     * @param init_list 初始化列表，元素将被拷贝
     */
    Vec(std::initializer_list<value_t>&& init_list) :
            size_(init_list.size()), capacity_(size_), data_(alloc_.allocate(capacity_)) {
        usize pos = 0;
        for (auto&& item : init_list) {
            alloc_.construct(data_ + pos, std::forward<decltype(item)>(item));
            pos++;
        }
    }

    /**
     * @brief 从可迭代对象构造
     * @tparam Iter 满足Iterable概念的类型
     * @param iter 可迭代对象，元素将被拷贝
     */
    template <Iterable I>
    Vec(I&& iter) :
            size_(iter.size()), capacity_(size_), data_(alloc_.allocate(capacity_)) {
        usize pos = 0;
        for (auto&& item : iter) {
            alloc_.construct(data_ + pos, std::forward<value_t>(item));
            pos++;
        }
    }

    /**
     * @brief 拷贝构造函数
     * @param other 被拷贝的向量
     */
    Vec(const Self& other) :
            alloc_(other.alloc_), size_(other.size_), capacity_(other.capacity_), data_(alloc_.allocate(capacity_)) {
        for (usize i = 0; i < size_; ++i) {
            alloc_.construct(data_ + i, other.data_[i]);
        }
    }

    /**
     * @brief 移动构造函数
     * @param other 被移动的向量
     */
    Vec(Self&& other) noexcept :
            alloc_(other.alloc_), size_(other.size_), capacity_(other.capacity_), data_(other.data_) {
        other.data_ = nullptr;
        other.size_ = other.capacity_ = 0;
    }

    /**
     * @brief 拷贝赋值运算符
     * @param other 数据来源
     * @return 自身引用
     */
    Self& operator=(const Self& other) {
        if (this == &other) return *this;

        alloc_.destroy(this);
        alloc_.construct(this, other);
        return *this;
    }

    /**
     * @brief 移动赋值运算符
     * @param other 数据来源
     * @return 自身引用
     */
    Self& operator=(Self&& other) noexcept {
        if (this == &other) return *this;

        alloc_.destroy(this);
        alloc_.construct(this, std::move(other));
        return *this;
    }

    /**
     * @brief 析构函数
     */
    ~Vec() {
        clear();
        if (data_) {
            alloc_.deallocate(data_, size_);
        }
        capacity_ = 0;
    }

    usize capacity() const {
        return capacity_;
    }

    /**
     * @brief 获取元素个数
     * @return 当前存储的元素个数
     */
    usize size() const noexcept {
        return size_;
    }

    /**
     * @brief 判断是否为空
     * @return size() == 0 时返回 true
     */
    bool empty() const noexcept {
        return size_ == 0;
    }

    value_t* data() {
        return data_;
    }

    const value_t* data() const {
        return data_;
    }

    /**
     * @brief 访问首元素（非空时有效）
     * @return 首元素的引用
     * @exception 空向量访问时行为未定义
     */
    value_t& front() noexcept {
        return data_[0];
    }

    const value_t& front() const noexcept {
        return data_[0];
    }

    /**
     * @brief 访问末元素（非空时有效）
     * @return 末元素的引用
     * @exception 空向量访问时行为未定义
     */
    value_t& back() noexcept {
        return data_[size_ - 1];
    }

    const value_t& back() const noexcept {
        return data_[size_ - 1];
    }

    /**
     * @brief 访问元素
     * @param idx 元素下标
     * @return 对应元素的引用
     * @exception 时间复杂度O(logN)。如果索引超出范围，行为未定义
     */
    value_t& at(usize idx) {
        return data_[idx];
    }

    const value_t& at(usize idx) const {
        return data_[idx];
    }

    usize find(const value_t& value) const {
        for (usize i = 0; i < size_; ++i) {
            if (data_[i] == value) {
                return i;
            }
        }
        return size_;
    }

    /**
     * @brief 拷贝/移动追加
     * @param item 要追加的元素
     * @return 被追加元素的引用
     */
    template <typename U>
    value_t& append(U&& item) {
        try_expand();
        alloc_.construct(data_ + size_, std::forward<U>(item));
        ++size_;
        return back();
    }

    /**
     * @brief 原地构造追加
     * @param args 构造元素的参数
     */
    template <typename... Args>
    value_t& append(Args&&... args) {
        try_expand();
        alloc_.construct(data_ + size_, std::forward<Args>(args)...);
        ++size_;
        return back();
    }

    /**
     * @brief 在指定位置插入元素
     * @tparam U 可转发类型
     * @param idx 插入位置，从0开始
     * @param item 要插入的元素
     */
    template <typename... Args>
    void insert(usize idx, Args&&... args) {
        if (idx > size_) return;
        try_expand();
        for (auto it = end(); it >= begin() + idx; --it) {
            *std::next(it) = std::move(*it);
        }
        alloc_.construct(data_ + idx, std::forward<Args>(args)...);
        ++size_;
    }

    /**
     * @brief 移除指定位置的元素
     * @param idx 移除位置，从 0 开始，默认移除最后一个元素
     */
    void pop(isize idx = -1) {
        if (empty()) return;

        idx = neg_index(idx, static_cast<isize>(size_));
        for (auto it = begin() + idx + 1; it != end(); ++it) {
            *std::prev(it) = std::move(*it);
        }
        alloc_.destroy(data_ + idx);
        --size_;
    }

    /**
     * @brief 清空所有元素（容量不变）
     */
    void clear() {
        alloc_.destroy(data_, size_);
        size_ = 0;
    }

    /**
     * @brief 交换两个向量内容
     * @param other 另一个向量
     */
    void swap(Self& other) noexcept {
        std::swap(alloc_, other.alloc_);
        std::swap(capacity_, other.capacity_);
        std::swap(size_, other.size_);
        std::swap(data_, other.data_);
    }

    /**
     * @brief 转换为 Array，复制元素
     * @return 返回包含所有元素的 Array
     */
    Array<value_t> to_array() const {
        Array<value_t> arr(size_);
        for (usize i = 0; i < size_; ++i) {
            arr[i] = data_[i];
        }
        return arr;
    }

    /**
     * @brief 转换为 Array，移动元素
     * @return 返回包含所有元素的 Array
     * @note 转换后原动态数组将被清空
     */
    Array<value_t> to_array() {
        Array<value_t> arr(size_);
        for (usize i = 0; i < size_; ++i) {
            arr[i] = std::move(data_[i]);
        }
        clear();
        return arr;
    }

    /**
     * @brief 向量切片 TODO 新增 VecView 类
     * @param start 起始索引
     * @param end 结束索引（不包含）
     * @return 子数组
     */
    Self slice(usize start, isize end) const {
        auto m_size = size();
        start = neg_index(start, m_size);
        end = neg_index(end, static_cast<isize>(m_size));
        Self ans(end - start);
        usize pos = 0;
        for (usize i = start; i < end; ++i) {
            ans.at(pos++) = at(i);
        }
        return ans;
    }

    /**
     * @brief 向量切片，返回从指定索引开始到末尾的子数组
     * @param start 起始索引
     * @return 子数组
     */
    Self slice(usize start) const {
        return slice(start, size());
    }

    /**
     * @brief 将一个可迭代对象追加到动态数组末尾
     * @tparam I 可迭代对象的类型
     * @param other 可迭代对象，其元素将被追加到动态数组末尾
     * @return 返回自身引用
     */
    template <Iterable I>
    Self& extend(I&& other) {
        for (auto&& item : other) {
            append(std::forward<decltype(item)>(item));
        }
        return *this;
    }

    /**
     * @brief 与另一个动态数组进行拼接
     * @param other 另一个动态数组
     * @return 返回拼接后的新动态数组
     */
    Self operator+(const Self& other) {
        return Self(*this).extend(other);
    }

    /**
     * @brief 与另一个动态数组进行拼接（右值引用版本）
     * @param other 另一个动态数组的右值引用
     * @return 返回拼接后的新动态数组
     */
    Self operator+(Self&& other) {
        return Self(*this).extend(std::forward<Self>(other));
    }

    /**
     * @brief 与另一个动态数组进行拼接
     * @param other 另一个动态数组
     * @return 返回自身引用
     */
    Self& operator+=(const Self& other) {
        return extend(other);
    }

    /**
     * @brief 与另一个动态数组进行拼接（右值引用版本）
     * @param other 另一个动态数组的右值引用
     * @return 返回自身引用
     */
    Self& operator+=(Self&& other) {
        return extend(std::forward<Self>(other));
    }

    /**
     * @brief 改变容量
     * @note 若新容量大于原容量，扩容到新容量并拷贝原向量的所有元素到新向量；
     * 若新容量小于原容量，缩容到新容量并拷贝原向量的前newsize个元素到新向量；
     * 若二者相等，则什么都不做
     */
    void resize(usize new_cap) {
        if (new_cap == capacity_) return;
        value_t* ptr = alloc_.allocate(new_cap);
        usize min_size = std::min(size_, new_cap);

        if constexpr (std::is_trivially_copyable_v<value_t>) {
            std::memcpy(ptr, data_, min_size * sizeof(value_t));
        } else {
            for (usize i = 0; i < min_size; ++i) {
                new (ptr + i) value_t(std::move(data_[i]));
                data_[i].~value_t();
            }
        }

        alloc_.deallocate(data_, size_);
        data_ = ptr;
        capacity_ = new_cap;
    }

    /**
     * @brief 分离数组，返回数组指针和大小，并将数组置空
     * @return 返回包含数组大小和指针的 Pair
     * @note 分离后，原数组将不再管理数组的内存，用户需要手动管理返回的指针
     */
    Pair<usize, value_t*> separate() {
        auto res = Pair{size_, data_};
        data_ = nullptr;
        size_ = capacity_ = 0;
        return res;
    }

    /**
     * @brief 预留存储空间
     * @param new_cap 新的容量
     */
    void reserve(usize new_cap) {
        if (new_cap > capacity_) {
            resize(new_cap);
        }
    }

    /**
     * @brief 获取动态数组的字符串表示
     * @return 返回动态数组的 CSV 格式的字符串
     */
    [[nodiscard]] CString __str__() const {
        std::stringstream stream;
        stream << '[';
        for (usize i = 0; i < size_; ++i) {
            if (i) stream << ',';
            stream << at(i);
        }
        stream << ']';
        return CString{stream.str()};
    }

    /**
     * @brief 迭代器支持
     * @tparam IsConst 是否为常量迭代器
     */
    template <bool IsConst>
    class Iterator : public Object<Iterator<IsConst>> {
        using Self = Iterator<IsConst>;

    public:
        using iterator_category = std::random_access_iterator_tag;
        using container_t = std::conditional_t<IsConst, const Vec<value_t>, Vec<value_t>>;
        using value_type = value_t;
        using difference_type = std::ptrdiff_t;
        using pointer = std::conditional_t<IsConst, const value_t*, value_t*>;
        using const_pointer = const value_type*;
        using reference = std::conditional_t<IsConst, const value_t&, value_t&>;
        using const_reference = const value_type&;

        /**
         * @brief 构造一个迭代器
         * @param dynarray 指向动态数组的指针
         * @param blockIndex 块索引
         * @param inblockIndex 块内索引
         */
        Iterator(container_t* vec_ptr = nullptr, usize cur_idx = 0) :
                vec_ptr_(vec_ptr), cur_idx_(cur_idx) {}

        /**
         * @brief 拷贝构造函数
         * @param other 需要拷贝的迭代器
         */
        Iterator(const Self& other) :
                Iterator(other.vec_ptr_, other.cur_idx_) {}

        /**
         * @brief 拷贝赋值操作符
         * @param other 需要拷贝的迭代器
         * @return 返回自身引用
         */
        Self& operator=(const Self& other) {
            this->cur_idx_ = other.cur_idx_;
            this->vec_ptr_ = other.vec_ptr_;
            return *this;
        }

        /**
         * @brief 解引用运算符
         * @return 返回当前元素的引用
         * @note 如果迭代器无效（如超过范围），行为未定义
         */
        reference operator*() {
            return vec_ptr_->at(cur_idx_);
        }

        /**
         * @brief 解引用运算符（const 版本）
         * @return 返回当前元素的 const 引用
         * @note 如果迭代器无效（如超过范围），行为未定义
         */
        const_reference operator*() const {
            return vec_ptr_->at(cur_idx_);
        }

        /**
         * @brief 获取指向当前元素的指针
         * @return 返回当前元素的指针
         * @note 如果迭代器无效（如超过范围），行为未定义
         */
        pointer operator->() {
            return &vec_ptr_->at(cur_idx_);
        }

        /**
         * @brief 获取指向当前元素的指针（const 版本）
         * @return 返回当前元素的 const 指针
         * @note 如果迭代器无效（如超过范围），行为未定义
         */
        const_pointer operator->() const {
            return &vec_ptr_->at(cur_idx_);
        }

        /**
         * @brief 前置自增运算符
         * 移动迭代器到下一个元素
         * @return 返回自增后的迭代器
         */
        Self& operator++() {
            ++cur_idx_;
            return *this;
        }

        /**
         * @brief 后置自增运算符
         * 移动迭代器到下一个元素
         * @return 返回自增前的迭代器
         */
        Self operator++(i32) {
            Self tmp(*this);
            ++tmp;
            return tmp;
        }

        /**
         * @brief 前置自减运算符
         * 移动迭代器到上一个元素
         * @return 返回自减后的迭代器
         */
        Self& operator--() {
            --cur_idx_;
            return *this;
        }

        /**
         * @brief 后置自减运算符
         * 移动迭代器到上一个元素
         * @return 返回自减前的迭代器
         */
        Self operator--(i32) {
            Self tmp(*this);
            --tmp;
            return tmp;
        }

        /**
         * @brief 跳过指定数量的元素
         * @param n 需要跳过的元素数量
         * @return 返回自身引用
         */
        Self& operator+=(difference_type n) {
            if (n == 0) return *this;

            cur_idx_ += n;
            return *this;
        }

        /**
         * @brief 跳过指定数量的元素
         * @param n 需要跳过的元素数量
         * @return 返回新的迭代器
         */
        Self operator+(difference_type n) const {
            Self tmp(*this);
            tmp += n;
            return tmp;
        }

        /**
         * @brief 回退指定数量的元素
         * @param n 需要回退的元素数量
         * @return 返回自身引用
         */
        Self& operator-=(difference_type n) {
            if (n == 0) return *this;

            cur_idx_ -= n;
            return *this;
        }

        /**
         * @brief 回退指定数量的元素
         * @param n 需要回退的元素数量
         * @return 返回新的迭代器
         */
        Self operator-(difference_type n) const {
            Self tmp(*this);
            tmp -= n;
            return tmp;
        }

        /**
         * @brief 获取两个迭代器的差值
         * @param other 另一个迭代器
         * @return 返回两个迭代器之间的元素数量差
         */
        difference_type operator-(const Self& other) const {
            if (this->__cmp__(other) < 0) return -(other - *this);
            if (this->vec_ptr_ != other.vec_ptr_) {
                throw runtime_exception("iterator not belong to the same container.");
            }

            return this->cur_idx_ - other.cur_idx_;
        }

        /**
         * @brief 比较两个迭代器是否相等
         * @param other 另一个迭代器
         * @return 如果相等返回 true，否则返回 false
         */
        bool operator==(const Self& other) const {
            return __equals__(other);
        }

        /**
         * @brief 比较两个迭代器是否不相等
         * @param other 另一个迭代器
         * @return 如果不相等返回 true，否则返回 false
         */
        bool operator!=(const Self& other) const {
            return !__equals__(other);
        }

        /**
         * @brief 比较两个迭代器的内部状态是否相等
         * @param other 另一个迭代器
         * @return 如果内部状态相等返回 true，否则返回 false
         */
        [[nodiscard]] bool __equals__(const Self& other) const {
            return this->cur_idx_ == other.cur_idx_ && this->vec_ptr_ == other.vec_ptr_;
        }

        /**
         * @brief 比较两个迭代器的顺序
         * @param other 另一个迭代器
         * @return 返回一个整数值，表示两个迭代器的顺序
         */
        [[nodiscard]] cmp_t __cmp__(const Self& other) const {
            return this->cur_idx_ - other.cur_idx_;
        }

    private:
        usize cur_idx_;        // 当前索引
        container_t* vec_ptr_; // 当前指向当前向量的指针
    };

    using iterator = Iterator<false>;
    using const_iterator = Iterator<true>;

    /**
     * @brief 获取动态数组的起始迭代器
     * @return 返回指向第一个元素的迭代器
     */
    iterator begin() {
        return iterator(this);
    }

    /**
     * @brief 获取动态数组的起始迭代器（const 版本）
     * @return 返回指向第一个元素的 const 迭代器
     */
    const_iterator begin() const {
        return const_iterator(this);
    }

    /**
     * @brief 获取动态数组的末尾迭代器
     * @return 返回指向最后一个元素之后的迭代器
     */
    iterator end() {
        return iterator(this, size_);
    }

    /**
     * @brief 获取动态数组的末尾迭代器（const 版本）
     * @return 返回指向最后一个元素之后的 const 迭代器
     */
    const_iterator end() const {
        return const_iterator(this, size_);
    }

private:
    fn try_expand() {
        if (size_ == capacity_) {
            resize(capacity_ << 1);
        }
    }

private:
    Alloc alloc_{};  // 内存分配器
    usize size_;     // 元素个数
    usize capacity_; // 总容量
    value_t* data_;  // 指向数据首地址的指针

    static constexpr usize DEFAULT_CAPACITY = 16;
};

/**
 * @brief 从动态数组中选择指定类型的参数
 * @tparam T 目标类型
 * @param args 动态数组
 * @param idx 参数索引
 * @return 返回目标类型的参数值，如果类型不匹配或索引超出范围则抛出ValueError
 */
template <typename T>
fn opt(const Vec<std::any>& args, usize idx)->T {
    if (idx < 0 || idx >= args.size()) {
        throw index_out_of_bounds_exception("index {} out of bounds [0..{}] in opt function.", SRC_LOC, idx, args.size());
    }

    try {
        return std::any_cast<T>(args.at(idx));
    } catch (const std::bad_any_cast& e) {
        throw type_exception("type mismatch in opt function: expected[{}], got[{}]", SRC_LOC, dtype(T), args[idx].type().name());
    }
}

} // namespace my::util

#endif // VEC_HPP