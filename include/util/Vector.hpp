/**
 * @brief 向量
 * @author Ricky
 * @date 2025/2/28
 * @version 1.0
 */
#ifndef VECTOR_HPP
#define VECTOR_HPP

#include "Sequence.hpp"

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
template <typename T>
class Vector : public Sequence<Vector<T>, T> {
public:
    using self = Vector<T>;
    using super = Sequence<Vector<T>, T>;
    using value_t = T;

    /**
     * @brief 默认构造函数
     * @note 创建一个空向量，容量为0
     */
    Vector() :
            size_(0), capacity_(DEFAULT_CAPACITY), data_(my_alloc<value_t>(capacity_)) {}

    /**
     * @brief 构造指定大小的向量并用默认值填充
     * @param size 初始元素个数
     * @param val 用于填充的值，默认为值初始化
     */
    Vector(isize size, const value_t& val = value_t{}) :
            size_(size), capacity_(size_), data_(my_alloc<value_t>(capacity_)) {
        for (isize i = 0; i < size_; ++i) {
            my_construct(data_ + i, val);
        }
    }

    /**
     * @brief 从初始化列表构造
     * @param initList 初始化列表，元素将被拷贝
     */
    Vector(std::initializer_list<value_t>&& initList) :
            size_(initList.size()), capacity_(size_), data_(my_alloc<value_t>(capacity_)) {
        isize pos = 0;
        for (auto&& item : initList) {
            my_construct(data_ + pos, std::forward<decltype(item)>(item));
            pos++;
        }
    }

    /**
     * @brief 从可迭代对象构造
     * @tparam Iter 满足Iterable概念的类型
     * @param iter 可迭代对象，元素将被拷贝
     */
    template <Iterable I>
    Vector(I&& iter) :
            size_(iter.size()), capacity_(size_), data_(my_alloc<value_t>(capacity_)) {
        isize pos = 0;
        for (auto&& item : iter) {
            my_construct(data_ + pos, std::forward<value_t>(item));
            pos++;
        }
    }

    /**
     * @brief 拷贝构造函数
     * @param other 被拷贝的向量
     */
    Vector(const self& other) :
            size_(other.size_), capacity_(other.capacity_), data_(my_alloc<value_t>(capacity_)) {
        for (isize i = 0; i < size_; ++i) {
            my_construct(data_ + i, other.data_[i]);
        }
    }

    /**
     * @brief 移动构造函数
     * @param other 被移动的向量
     */
    Vector(self&& other) noexcept :
            size_(other.size_), capacity_(other.capacity_), data_(other.data_) {
        other.data_ = nullptr;
        other.size_ = other.capacity_ = 0;
    }

    /**
     * @brief 拷贝赋值运算符
     * @param other 数据来源
     * @return 自身引用
     */
    self& operator=(const self& other) {
        if (this == &other) return *this;

        my_destroy(this);
        return *my_construct(this, other);
    }

    /**
     * @brief 移动赋值运算符
     * @param other 数据来源
     * @return 自身引用
     */
    self& operator=(self&& other) noexcept {
        if (this == &other) return *this;

        my_destroy(this);
        return *my_construct(this, std::move(other));
    }

    /**
     * @brief 析构函数
     */
    ~Vector() {
        clear();
        if (data_) {
            my_delloc(data_);
        }
        capacity_ = 0;
    }

    /**
     * @brief 获取元素个数
     * @return 当前存储的元素个数
     */
    isize size() const noexcept {
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
     * @param index 元素下标
     * @return 对应元素的引用
     * @exception 时间复杂度O(logN)。如果索引超出范围，行为未定义
     */
    value_t& at(isize index) {
        return data_[index];
    }

    const value_t& at(isize index) const {
        return data_[index];
    }

    isize find(const value_t& value) const {
        for (isize i = 0; i < size_; ++i) {
            if (data_[i] == value) {
                return i;
            }
        }
        return size_;
    }

    /**
     * @brief 在末尾追加元素（拷贝语义）
     * @param item 要追加的元素
     * @return 被追加元素的引用
     */
    value_t& append(const value_t& item) {
        try_expand();
        my_construct(data_ + size_, item);
        ++size_;
        return back();
    }

    /**
     * @brief 在末尾追加元素（移动语义）
     * @param item 要移动的元素
     * @return 被追加元素的引用
     */
    template <typename... Args>
    value_t& append(Args&&... args) {
        try_expand();
        my_construct(data_ + size_, std::forward<Args>(args)...);
        ++size_;
        return back();
    }

    /**
     * @brief 在指定位置插入元素
     * @tparam U 可转发类型
     * @param index 插入位置，从0开始
     * @param item 要插入的元素
     */
    template <typename... Args>
    void insert(isize index, Args&&... args) {
        if (index < 0 || index > size_) return;
        try_expand();
        for (isize i = size_ - 1; i >= index; --i) {
            data_[i + 1] = std::move(data_[i]);
        }
        my_construct(data_ + index, std::forward<Args>(args)...);
        ++size_;
    }

    /**
     * @brief 移除指定位置的元素
     * @param index 移除位置，从 0 开始，默认移除最后一个元素
     */
    void pop(isize index = -1) {
        if (empty()) return;

        index = neg_index(index, size_);
        for (isize i = index + 1; i < size_; ++i) {
            data_[i - 1] = std::move(data_[i]);
        }
        my_destroy(data_ + index);
        --size_;
    }

    /**
     * @brief 清空所有元素（容量不变）
     */
    void clear() {
        my_destroy(data_, size_);
        size_ = 0;
    }

    /**
     * @brief 交换两个向量内容
     * @param other 另一个向量
     */
    void swap(self& other) noexcept {
        std::swap(data_, other.data_);
        std::swap(size_, other.size_);
        std::swap(capacity_, other.capacity_);
    }

    /**
     * @brief 转换为 Array，复制元素
     * @return 返回包含所有元素的 Array
     */
    Array<value_t> toArray() const {
        Array<value_t> arr(size_);
        for (isize i = 0; i < size_; ++i) {
            arr[i] = data_[i];
        }
        return arr;
    }

    /**
     * @brief 转换为 Array，移动元素
     * @return 返回包含所有元素的 Array
     * @note 转换后原动态数组将被清空
     */
    Array<value_t> toArray() {
        Array<value_t> arr(size_);
        for (isize i = 0; i < size_; ++i) {
            arr[i] = std::move(data_[i]);
        }
        clear();
        return arr;
    }

    /**
     * @brief 将一个可迭代对象追加到动态数组末尾
     * @tparam I 可迭代对象的类型
     * @param other 可迭代对象，其元素将被追加到动态数组末尾
     * @return 返回自身引用
     */
    template <Iterable I>
    self& extend(I&& other) {
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
    self operator+(const self& other) {
        return self(*this).extend(other);
    }

    /**
     * @brief 与另一个动态数组进行拼接（右值引用版本）
     * @param other 另一个动态数组的右值引用
     * @return 返回拼接后的新动态数组
     */
    self operator+(self&& other) {
        return self(*this).extend(std::forward<self>(other));
    }

    /**
     * @brief 与另一个动态数组进行拼接
     * @param other 另一个动态数组
     * @return 返回自身引用
     */
    self& operator+=(const self& other) {
        return extend(other);
    }

    /**
     * @brief 与另一个动态数组进行拼接（右值引用版本）
     * @param other 另一个动态数组的右值引用
     * @return 返回自身引用
     */
    self& operator+=(self&& other) {
        return extend(std::forward<self>(other));
    }

    /**
     * @brief 改变容量
     * @note 若新容量大于原容量，扩容到新容量并拷贝原向量的所有元素到新向量；
     * 若新容量小于原容量，缩容到新容量并拷贝原向量的前newsize个元素到新向量；
     * 若二者相等，则什么都不做
     */
    void resize(isize newCapacity) {
        if (newCapacity == capacity_) return;
        value_t* ptr = my_alloc<value_t>(newCapacity);
        isize minSize = math::min_(size_, newCapacity);

        if constexpr (std::is_trivially_copyable_v<value_t>) {
            std::memcpy(ptr, data_, minSize * sizeof(value_t));
        } else {
            for (isize i = 0; i < minSize; ++i) {
                new (ptr + i) value_t(std::move(data_[i])); // 移动构造
                data_[i].~value_t();
            }
        }

        my_delloc(data_);
        data_ = ptr;
        capacity_ = newCapacity;
    }

    /**
     * @brief 分离数组，返回数组指针和大小，并将数组置空
     * @return 返回包含数组大小和指针的 Pair
     * @note 分离后，原数组将不再管理数组的内存，用户需要手动管理返回的指针
     */
    Pair<isize, value_t*> separate() {
        auto res = Pair{size_, data_};
        data_ = nullptr;
        size_ = capacity_ = 0;
        return res;
    }

    /**
     * @brief 扩容
     */
    void reserve(isize new_capacity) {
        if (new_capacity > capacity_) {
            resize(new_capacity);
        }
    }

    /**
     * @brief 获取动态数组的字符串表示
     * @return 返回动态数组的 CSV 格式的字符串
     */
    CString __str__() const {
        std::stringstream stream;
        stream << '[';
        for (isize i = 0; i < size_; ++i) {
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
        using self = Iterator<IsConst>;

    public:
        using container_t = std::conditional_t<IsConst, const Vector<value_t>, Vector<value_t>>;
        using iterator_category = std::random_access_iterator_tag;
        using value_type = std::conditional_t<IsConst, const value_t, value_t>;
        using difference_type = std::ptrdiff_t;
        using pointer = value_type*;
        using const_pointer = const value_type*;
        using reference = value_type&;
        using const_reference = const value_type&;

        /**
         * @brief 构造一个迭代器
         * @param dynarray 指向动态数组的指针
         * @param blockIndex 块索引
         * @param inblockIndex 块内索引
         */
        Iterator(container_t* vectorPtr = nullptr, isize currentIndex = 0) :
                vectorPtr_(vectorPtr), currentIndex_(currentIndex) {}

        /**
         * @brief 拷贝构造函数
         * @param other 需要拷贝的迭代器
         */
        Iterator(const self& other) :
                Iterator(other.vectorPtr_, other.currentIndex_) {}

        /**
         * @brief 拷贝赋值操作符
         * @param other 需要拷贝的迭代器
         * @return 返回自身引用
         */
        self& operator=(const self& other) {
            this->currentIndex_ = other.currentIndex_;
            this->vectorPtr_ = other.vectorPtr_;
            return *this;
        }

        /**
         * @brief 解引用运算符
         * @return 返回当前元素的引用
         * @note 如果迭代器无效（如超过范围），行为未定义
         */
        reference operator*() {
            return vectorPtr_->at(currentIndex_);
        }

        /**
         * @brief 解引用运算符（const 版本）
         * @return 返回当前元素的 const 引用
         * @note 如果迭代器无效（如超过范围），行为未定义
         */
        const_reference operator*() const {
            return vectorPtr_->at(currentIndex_);
        }

        /**
         * @brief 获取指向当前元素的指针
         * @return 返回当前元素的指针
         * @note 如果迭代器无效（如超过范围），行为未定义
         */
        pointer operator->() {
            return &vectorPtr_->at(currentIndex_);
        }

        /**
         * @brief 获取指向当前元素的指针（const 版本）
         * @return 返回当前元素的 const 指针
         * @note 如果迭代器无效（如超过范围），行为未定义
         */
        const_pointer operator->() const {
            return &vectorPtr_->at(currentIndex_);
        }

        /**
         * @brief 前置自增运算符
         * 移动迭代器到下一个元素
         * @return 返回自增后的迭代器
         */
        self& operator++() {
            ++currentIndex_;
            return *this;
        }

        /**
         * @brief 后置自增运算符
         * 移动迭代器到下一个元素
         * @return 返回自增前的迭代器
         */
        self operator++(int) {
            self tmp(*this);
            ++tmp;
            return tmp;
        }

        /**
         * @brief 前置自减运算符
         * 移动迭代器到上一个元素
         * @return 返回自减后的迭代器
         */
        self& operator--() {
            --currentIndex_;
            return *this;
        }  

        /**
         * @brief 后置自减运算符
         * 移动迭代器到上一个元素
         * @return 返回自减前的迭代器
         */
        self operator--(int) {
            self tmp(*this);
            --tmp;
            return tmp;
        }

        /**
         * @brief 跳过指定数量的元素
         * @param n 需要跳过的元素数量
         * @return 返回自身引用
         */
        self& operator+=(difference_type n) {
            if (n == 0) return *this;

            currentIndex_ += n;
            return *this;
        }

        /**
         * @brief 跳过指定数量的元素
         * @param n 需要跳过的元素数量
         * @return 返回新的迭代器
         */
        self operator+(difference_type n) const {
            self tmp(*this);
            tmp += n;
            return tmp;
        }

        /**
         * @brief 回退指定数量的元素
         * @param n 需要回退的元素数量
         * @return 返回自身引用
         */
        self& operator-=(difference_type n) {
            if (n == 0) return *this;

            currentIndex_ -= n;
            return *this;
        }

        /**
         * @brief 回退指定数量的元素
         * @param n 需要回退的元素数量
         * @return 返回新的迭代器
         */
        self operator-(difference_type n) const {
            self tmp(*this);
            tmp -= n;
            return tmp;
        }

        /**
         * @brief 获取两个迭代器的差值
         * @param other 另一个迭代器
         * @return 返回两个迭代器之间的元素数量差
         */
        difference_type operator-(const self& other) const {
            if (this->__cmp__(other) < 0) return -(other - *this);
            if (this->vectorPtr_ != other.vectorPtr_) {
                RuntimeError("Iterator not belong to the same container.");
            }

            return this->currentIndex_ - other.currentIndex_;
        }

        /**
         * @brief 比较两个迭代器是否相等
         * @param other 另一个迭代器
         * @return 如果相等返回 true，否则返回 false
         */
        bool operator==(const self& other) const {
            return __equals__(other);
        }

        /**
         * @brief 比较两个迭代器是否不相等
         * @param other 另一个迭代器
         * @return 如果不相等返回 true，否则返回 false
         */
        bool operator!=(const self& other) const {
            return !__equals__(other);
        }

        /**
         * @brief 比较两个迭代器的内部状态是否相等
         * @param other 另一个迭代器
         * @return 如果内部状态相等返回 true，否则返回 false
         */
        bool __equals__(const self& other) const {
            return this->currentIndex_ == other.currentIndex_ && this->vectorPtr_ == other.vectorPtr_;
        }

        /**
         * @brief 比较两个迭代器的顺序
         * @param other 另一个迭代器
         * @return 返回一个整数值，表示两个迭代器的顺序
         */
        cmp_t __cmp__(const self& other) const {
            return this->currentIndex_ - other.currentIndex_;
        }

    private:
        isize currentIndex_;     // 当前索引
        container_t* vectorPtr_; // 当前指向当前向量的指针
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
    isize size_;     // 元素个数
    isize capacity_; // 总容量
    value_t* data_;  // 指向数据首地址的指针

    static constexpr isize DEFAULT_CAPACITY = 16;

private:
    void try_expand() {
        if (size_ == capacity_) {
            resize(capacity_ << 1);
        }
    }
};

} // namespace my::util

#endif // VECTOR_HPP