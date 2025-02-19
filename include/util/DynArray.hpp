/**
 * @brief 动态数组
 * @author Ricky
 * @date 2024/12/12
 * @version 1.0
 */
#ifndef DYN_ARRAY_HPP
#define DYN_ARRAY_HPP

#include "binary_utils.hpp"
#include "Buffer.hpp"
#include "Array.hpp"

#include <any>

namespace my::util {

/**
 * @class DynArray
 * @brief 动态数组类，通过分块管理内存，支持高效的随机访问和动态扩容。
 * 
 * DynArray 是一个模板类，提供了类似 std::vector 的动态数组功能。通过将数据分块存储来管理内存，一定程度上优化了内存分配和访问效率。
 * 
 * @tparam T 存储元素的类型。
 */
template <typename T>
class DynArray : public Sequence<DynArray<T>, T> {
    using self = DynArray<T>;
    using super = Sequence<DynArray<T>, T>;

    // 动态数组块的数量，用于管理分块存储
    constexpr static i32 DYNARRAY_BLOCK_SIZE = 63;

    // 最小块大小，每个块的初始大小
    constexpr static c_size BASE_SIZE = 8ll;

    // 不存在块时的特殊值
    constexpr static i32 BLOCK_NOT_EXISTS = -1;

public:
    using value_t = T;

    /**
     * @brief 默认构造函数。
     * 初始化为空的动态数组，无元素。
     */
    DynArray() :
            size_(0), backBlockIndex_(BLOCK_NOT_EXISTS), blocks_(DYNARRAY_BLOCK_SIZE, 0) {}

    /**
     * @brief 构造指定大小的动态数组，并用指定的元素进行填充。
     * @param size 动态数组的大小。
     * @param item 用于填充数组的元素，默认为空值。
     */
    DynArray(c_size size, const value_t& item = value_t{}) :
            DynArray() {
        for (c_size i = 0; i < size; ++i) {
            append(item);
        }
    }

    /**
     * @brief 使用初始化列表构造动态数组。
     * @param init 初始化列表，包含数组的初始元素。
     */
    DynArray(std::initializer_list<value_t>&& init) :
            DynArray() {
        for (auto& item : init) {
            append(std::forward<decltype(item)>(item));
        }
    }

    /**
     * @brief 使用可迭代对象构造动态数组。
     * @tparam I 可迭代对象的类型。
     * @param other 可迭代对象，其元素将被复制到动态数组中。
     */
    template <Iterable I>
    DynArray(I&& other) :
            DynArray() {
        for (auto&& item : other) {
            append(std::forward<decltype(item)>(item));
        }
    }

    /**
     * @brief 拷贝构造函数。
     * @param other 需要拷贝的动态数组。
     */
    DynArray(const self& other) :
            DynArray() {
        for (const auto& item : other) {
            append(item);
        }
    }

    /**
     * @brief 移动构造函数。
     * @param other 需要移动的动态数组。
     */
    DynArray(self&& other) noexcept :
            size_(other.size_), backBlockIndex_(other.backBlockIndex_), blocks_(std::move(other.blocks_)) {
        other.size_ = 0;
        other.backBlockIndex_ = BLOCK_NOT_EXISTS;
    }

    /**
     * @brief 拷贝赋值操作符。
     * @param other 需要拷贝的动态数组。
     * @return 返回拷贝后的动态数组。
     */
    self& operator=(const self& other) {
        if (this == &other) return *this;

        this->size_ = other.size_;
        this->backBlockIndex_ = other.backBlockIndex_;
        this->blocks_ = other.blocks_;
        return *this;
    }

    /**
     * @brief 移动赋值操作符。
     * @param other 需要移动的动态数组。
     * @return 返回移动后的动态数组。
     */
    self& operator=(self&& other) noexcept {
        if (this == &other) return *this;

        this->size_ = other.size_;
        this->backBlockIndex_ = other.backBlockIndex_;
        this->blocks_ = std::move(other.blocks_);

        other.size_ = 0;
        other.backBlockIndex_ = BLOCK_NOT_EXISTS;

        return *this;
    }

    /**
     * @brief 析构函数。
     */
    ~DynArray() = default;

    /**
     * @brief 获取动态数组的大小。
     * @return 返回数组中元素的个数。
     */
    c_size size() const {
        return size_;
    }

    /**
     * @brief 检查动态数组是否为空。
     * @return 如果为空返回 true，否则返回 false。
     */
    bool empty() const {
        return size_ == 0;
    }

    /**
     * @brief 获取动态数组的第一个元素。
     * @return 返回第一个元素的引用。
     * @note 如果数组为空，行为未定义。
     */
    value_t& front() {
        return at(0);
    }

    /**
     * @brief 获取动态数组的第一个元素（常量版本）。
     * @return 返回第一个元素的 const 引用。
     * @note 如果数组为空，行为未定义。
     */
    const value_t& front() const {
        return at(0);
    }

    /**
     * @brief 获取动态数组的最后一个元素。
     * @return 返回最后一个元素的引用。
     * @note 如果数组为空，行为未定义。
     */
    value_t& back() {
        return at(size() - 1);
    }

    /**
     * @brief 获取动态数组的最后一个元素（常量版本）。
     * @return 返回最后一个元素的 const 引用。
     * @note 如果数组为空，行为未定义。
     */
    const value_t& back() const {
        return at(size() - 1);
    }

    /**
     * @brief 通过索引访问元素（非 const 版本）。
     * @param index 元素的索引，从 0 开始。
     * @return 返回指定索引处的元素的引用。
     * @note 如果索引超出范围，行为未定义。
     */
    value_t& at(c_size index) {
        i32 blockIndex = get_block_index(index + 1);
        c_size inblockIndex = get_inblock_index(index + 1, blockIndex);
        return blocks_.at(blockIndex).at(inblockIndex);
    }

    /**
     * @brief 通过索引访问元素（const 版本）。
     * @param index 元素的索引，从 0 开始。
     * @return 返回指定索引处的元素的 const 引用。
     * @note 如果索引超出范围，行为未定义。
     */
    const value_t& at(c_size index) const {
        i32 blockIndex = get_block_index(index + 1);
        c_size inblockIndex = get_inblock_index(index + 1, blockIndex);
        return blocks_.at(blockIndex).at(inblockIndex);
    }

    /**
     * @brief 线性查找元素。
     * @param value 需要查找的元素值。
     * @return 如果存在返回第一个匹配元素的索引，否则返回数组的大小。
     * @note 查找时间复杂度为 O(n)，n 为数组大小。
     */
    c_size find(const value_t& value) const {
        for (c_size i = 0; i < size_; ++i) {
            if (at(i) == value) {
                return i;
            }
        }
        return size_;
    }

    /**
     * @brief 在动态数组末尾追加元素。
     * @tparam U 元素的类型。
     * @param item 需要追加的元素。
     * @return 返回追加的元素的引用。
     */
    template <typename U>
    value_t& append(U&& item) {
        try_wakeup();
        auto& res = back_block().append(std::forward<U>(item));
        ++size_;
        return res;
    }

    /**
     * @brief 在指定位置插入元素。
     * @tparam U 元素的类型。
     * @param index 插入位置，从 0 开始。
     * @param item 需要插入的元素。
     */
    template <typename U>
    void insert(c_size index, U&& item) {
        append(std::forward<U>(item));
        for (c_size i = size() - 1; i > index; --i) {
            std::swap(at(i), at(i - 1));
        }
    }

    /**
     * @brief 移除指定位置的元素。
     * @param index 移除位置，从 0 开始，默认移除最后一个元素。
     */
    void pop(c_size index = -1) {
        if (empty()) {
            return;
        }

        c_size m_size = size();
        index = neg_index(index, m_size);

        for (c_size i = index + 1; i < m_size; ++i) {
            at(i - 1) = std::move(at(i));
        }

        auto& backBlock = back_block();
        if (backBlock.size() == 1) {
            pop_back_block();
        } else {
            backBlock.pop_back();
        }
        --size_;
    }

    /**
     * @brief 清空动态数组。
     */
    void clear() {
        if (blocks_.empty()) {
            return;
        }

        for (auto&& block : blocks_) {
            block.resize(0);
        }
        size_ = 0;
        backBlockIndex_ = BLOCK_NOT_EXISTS;
    }

    /**
     * @brief 转换为 Array，复制元素。
     * @return 返回包含所有元素的 Array。
     */
    Array<value_t> toArray() const {
        c_size m_size = size();
        Array<value_t> arr(m_size);
        for (c_size i = 0; i < m_size; ++i) {
            arr.at(i) = at(i);
        }
        return arr;
    }

    /**
     * @brief 转换为 Array，移动元素。
     * @return 返回包含所有元素的 Array。
     * @note 转换后原动态数组将被清空。
     */
    Array<value_t> toArray() {
        c_size m_size = size();
        Array<value_t> arr(m_size);
        for (c_size i = 0; i < m_size; ++i) {
            arr.at(i) = std::move(at(i));
        }
        clear();
        return arr;
    }

    /**
     * @brief 将一个可迭代对象追加到动态数组末尾。
     * @tparam I 可迭代对象的类型。
     * @param other 可迭代对象，其元素将被追加到动态数组末尾。
     * @return 返回自身引用。
     */
    template <Iterable I>
    self& extend(I&& other) {
        for (auto&& item : other) {
            append(std::forward<decltype(item)>(item));
        }
        return *this;
    }

    /**
     * @brief 与另一个动态数组进行拼接。
     * @param other 另一个动态数组。
     * @return 返回拼接后的新动态数组。
     */
    self operator+(const self& other) {
        return self(*this).extend(other);
    }

    /**
     * @brief 与另一个动态数组进行拼接（右值引用版本）。
     * @param other 另一个动态数组的右值引用。
     * @return 返回拼接后的新动态数组。
     */
    self operator+(self&& other) {
        return self(*this).extend(std::forward<self>(other));
    }

    /**
     * @brief 与另一个动态数组进行拼接。
     * @param other 另一个动态数组。
     * @return 返回自身引用。
     */
    self& operator+=(const self& other) {
        return extend(other);
    }

    /**
     * @brief 与另一个动态数组进行拼接（右值引用版本）。
     * @param other 另一个动态数组的右值引用。
     * @return 返回自身引用。
     */
    self& operator+=(self&& other) {
        return extend(std::forward<self>(other));
    }

    /**
     * @brief 获取动态数组的字符串表示。
     * @return 返回动态数组的 CSV 格式的字符串。
     */
    CString __str__() const {
        std::stringstream stream;
        stream << '[';
        for (c_size i = 0, m_size = size(); i < m_size; ++i) {
            if (i) stream << ',';
            stream << at(i);
        }
        stream << ']';
        return CString{stream.str()};
    }

    /**
     * @brief 迭代器。
     * @tparam IsConst 是否为常量迭代器
     */
    template <bool IsConst>
    class Iterator : public Object<Iterator<IsConst>> {
        using self = Iterator<IsConst>;

    public:
        using container_t = std::conditional_t<IsConst, const DynArray<value_t>, DynArray<value_t>>;
        using iterator_category = std::random_access_iterator_tag;
        using value_type = std::conditional_t<IsConst, const value_t, value_t>;
        using difference_type = std::ptrdiff_t;
        using pointer = value_type*;
        using const_pointer = const value_type*;
        using reference = value_type&;
        using const_reference = const value_type&;

        /**
         * @brief 构造一个迭代器。
         * @param dynarray 指向动态数组的指针。
         * @param blockIndex 块索引。
         * @param inblockIndex 块内索引。
         */
        Iterator(container_t* dynarray = nullptr, c_size blockIndex = 0, c_size inblockIndex = 0) :
                blockIndex_(blockIndex), inblockIndex_(inblockIndex), dynarray_(dynarray) {}

        /**
         * @brief 拷贝构造函数。
         * @param other 需要拷贝的迭代器。
         */
        Iterator(const self& other) :
                Iterator(other.dynarray_, other.blockIndex_, other.inblockIndex_) {}

        /**
         * @brief 拷贝赋值操作符。
         * @param other 需要拷贝的迭代器。
         * @return 返回自身引用。
         */
        self& operator=(const self& other) {
            this->blockIndex_ = other.blockIndex_;
            this->inblockIndex_ = other.inblockIndex_;
            this->dynarray_ = other.dynarray_;
            return *this;
        }

        /**
         * @brief 解引用运算符。
         * @return 返回当前元素的引用。
         * @note 如果迭代器无效（如超过范围），行为未定义。
         */
        reference operator*() {
            return dynarray_->blocks_.at(blockIndex_).at(inblockIndex_);
        }

        /**
         * @brief 解引用运算符（const 版本）。
         * @return 返回当前元素的 const 引用。
         * @note 如果迭代器无效（如超过范围），行为未定义。
         */
        const_reference operator*() const {
            return dynarray_->blocks_.at(blockIndex_).at(inblockIndex_);
        }

        /**
         * @brief 获取指向当前元素的指针。
         * @return 返回当前元素的指针。
         * @note 如果迭代器无效（如超过范围），行为未定义。
         */
        pointer operator->() {
            return &dynarray_->blocks_.at(blockIndex_).at(inblockIndex_);
        }

        /**
         * @brief 获取指向当前元素的指针（const 版本）。
         * @return 返回当前元素的 const 指针。
         * @note 如果迭代器无效（如超过范围），行为未定义。
         */
        const_pointer operator->() const {
            return &dynarray_->blocks_.at(blockIndex_).at(inblockIndex_);
        }

        /**
         * @brief 前置自增运算符。
         * 移动迭代器到下一个元素。
         * @return 返回自增后的迭代器。
         */
        self& operator++() {
            if (inblockIndex_ == dynarray_->blocks_.at(blockIndex_).size() - 1) {
                ++blockIndex_;
                inblockIndex_ = 0;
                return *this;
            }
            ++inblockIndex_;
            return *this;
        }

        /**
         * @brief 后置自增运算符。
         * 移动迭代器到下一个元素。
         * @return 返回自增前的迭代器。
         */
        self operator++(int) {
            self tmp(*this);
            ++tmp;
            return tmp;
        }

        /**
         * @brief 前置自减运算符。
         * 移动迭代器到上一个元素。
         * @return 返回自减后的迭代器。
         */
        self& operator--() {
            if (dynarray_->blocks_.at(blockIndex_).size() == 0) {
                --blockIndex_;
                inblockIndex_ = dynarray_->blocks_.at(blockIndex_).size() - 1;
                return *this;
            }
            --inblockIndex_;
            return *this;
        }

        /**
         * @brief 后置自减运算符。
         * 移动迭代器到上一个元素。
         * @return 返回自减前的迭代器。
         */
        self operator--(int) {
            self tmp(*this);
            --tmp;
            return tmp;
        }

        /**
         * @brief 跳过指定数量的元素。
         * @param n 需要跳过的元素数量。
         * @return 返回自身引用。
         */
        self& operator+=(difference_type n) {
            if (n == 0) return *this;

            c_size target = (blockIndex_ * exp2[blockIndex_ + 1] - 1) * BASE_SIZE + inblockIndex_ + n;
            i32 new_block = dynarray_->get_block_index(target + 1);
            c_size new_inblock = dynarray_->get_inblock_index(target, new_block);

            if (new_block >= dynarray_->blocks_.size()) {
                new_block = dynarray_->blocks_.size() - 1;
                new_inblock = dynarray_->blocks_.at(new_block).size();
            }

            blockIndex_ = new_block;
            inblockIndex_ = new_inblock < dynarray_->blocks_.at(new_block).size() ? new_inblock : dynarray_->blocks_.at(new_block).size();

            return *this;
        }

        /**
         * @brief 跳过指定数量的元素。
         * @param n 需要跳过的元素数量。
         * @return 返回新的迭代器。
         */
        self operator+(difference_type n) const {
            self tmp(*this);
            tmp += n;
            return tmp;
        }

        /**
         * @brief 回退指定数量的元素。
         * @param n 需要回退的元素数量。
         * @return 返回自身引用。
         */
        self& operator-=(difference_type n) {
            if (n == 0) return *this;

            c_size target = (blockIndex_ * exp2[blockIndex_ + 1] - 1) * BASE_SIZE + inblockIndex_ - n;
            i32 new_block = dynarray_->get_block_index(target + 1);
            c_size new_inblock = dynarray_->get_inblock_index(target, new_block);

            if (new_block < 0) {
                new_block = 0;
                new_inblock = 0;
            }

            if (new_block < dynarray_->blocks_.size()) {
                inblockIndex_ = new_inblock;
                blockIndex_ = new_block;
            } else {
                blockIndex_ = dynarray_->blocks_.size() - 1;
                inblockIndex_ = dynarray_->blocks_.at(blockIndex_).size();
            }

            return *this;
        }

        /**
         * @brief 回退指定数量的元素。
         * @param n 需要回退的元素数量。
         * @return 返回新的迭代器。
         */
        self operator-(difference_type n) const {
            self tmp(*this);
            tmp -= n;
            return tmp;
        }

        /**
         * @brief 获取两个迭代器的差值。
         * @param other 另一个迭代器。
         * @return 返回两个迭代器之间的元素数量差。
         */
        difference_type operator-(const self& other) const {
            if (this->__cmp__(other) < 0) return -(other - *this);
            if (this->dynarray_ != other.dynarray_) {
                RuntimeError("Iterator not belong to the same container.");
            }

            if (this->blockIndex_ == other.blockIndex_) {
                return this->inblockIndex_ - other.inblockIndex_;
            }
            difference_type diff = this->dynarray_->blocks_.at(other.blockIndex_).size() - other.inblockIndex_;
            for (c_size i = other.blockIndex_ + 1; i < this->blockIndex_; ++i) {
                diff += this->dynarray_->blocks_.at(i).size();
            }
            return diff + this->inblockIndex_;
        }

        /**
         * @brief 比较两个迭代器是否相等。
         * @param other 另一个迭代器。
         * @return 如果相等返回 true，否则返回 false。
         */
        bool operator==(const self& other) const {
            return __equals__(other);
        }

        /**
         * @brief 比较两个迭代器是否不相等。
         * @param other 另一个迭代器。
         * @return 如果不相等返回 true，否则返回 false。
         */
        bool operator!=(const self& other) const {
            return !__equals__(other);
        }

        /**
         * @brief 比较两个迭代器的内部状态是否相等。
         * @param other 另一个迭代器。
         * @return 如果内部状态相等返回 true，否则返回 false。
         */
        bool __equals__(const self& other) const {
            return this->blockIndex_ == other.blockIndex_ && this->inblockIndex_ == other.inblockIndex_ && this->dynarray_ == other.dynarray_;
        }

        /**
         * @brief 比较两个迭代器的顺序。
         * @param other 另一个迭代器。
         * @return 返回一个整数值，表示两个迭代器的顺序。
         */
        cmp_t __cmp__(const self& other) const {
            if (this->blockIndex_ != other.blockIndex_) {
                return this->blockIndex_ - other.blockIndex_;
            }
            return this->inblockIndex_ - other.inblockIndex_;
        }

    private:
        c_size blockIndex_;     // 当前块索引
        c_size inblockIndex_;   // 当前块内的索引
        container_t* dynarray_; // 当前指向当前动态数组的指针
    };

    using iterator = Iterator<false>;
    using const_iterator = Iterator<true>;

    /**
     * @brief 获取动态数组的起始迭代器。
     * @return 返回指向第一个元素的迭代器。
     */
    iterator begin() {
        return iterator(this);
    }

    /**
     * @brief 获取动态数组的起始迭代器（const 版本）。
     * @return 返回指向第一个元素的 const 迭代器。
     */
    const_iterator begin() const {
        return const_iterator(this);
    }

    /**
     * @brief 获取动态数组的末尾迭代器。
     * @return 返回指向最后一个元素之后的迭代器。
     */
    iterator end() {
        return iterator(this, back_block_index() + 1);
    }

    /**
     * @brief 获取动态数组的末尾迭代器（const 版本）。
     * @return 返回指向最后一个元素之后的 const 迭代器。
     */
    const_iterator end() const {
        return const_iterator(this, back_block_index() + 1);
    }

private:
    /**
     * @brief 获取第 ith 个元素所在的块索引。
     * @param ith 元素在动态数组中的序号（从 1 开始）。
     * @return 返回块索引。
     * @note 时间复杂度为 O(log n)。
     */
    i32 get_block_index(c_size ith) const {
        i32 l = 0, r = DYNARRAY_BLOCK_SIZE, mid;
        while (l < r) {
            mid = l + ((r - l) >> 1);
            if (ith <= (exp2[mid + 1] - 1) * BASE_SIZE) {
                r = mid;
            } else {
                l = mid + 1;
            }
        }
        return l;
    }

    /**
     * @brief 获取第 ith 个元素在块内的索引。
     * @param ith 元素在动态数组中的序号（从 1 开始）。
     * @param blockIndex 块索引。
     * @return 返回块内索引。
     */
    c_size get_inblock_index(c_size ith, i32 blockIndex) const {
        return ith - BASE_SIZE * (exp2[blockIndex] - 1) - 1;
    }

    /**
     * @brief 获取最后一个块的索引。
     * @return 返回最后一个块的索引，如果没有块返回 -1。
     */
    i32 back_block_index() const {
        return backBlockIndex_;
    }

    /**
     * @brief 获取最后一个块。
     * @return 返回指向最后一个块的引用。
     * @note 如果没有块，行为未定义。
     */
    Buffer<T>& back_block() {
        return blocks_.at(back_block_index());
    }

    /**
     * @brief 移除最后一个块。
     */
    void pop_back_block() {
        back_block().resize(0);
        --backBlockIndex_;
    }

    /**
     * @brief 尝试唤醒一个新的块，用于扩展动态数组的存储空间。
     * 
     * 当动态数组的最后一个块已满或不存在时，会创建一个新的块，并调整其大小以容纳更多元素。
     * 这个函数不会导致数组本身的扩容，只会影响内部块的管理。
     */
    void try_wakeup() {
        i32 bbi = back_block_index(); // 获取最后一个块的索引，如果不存在，返回 BLOCK_NOT_EXISTS
        if (bbi == BLOCK_NOT_EXISTS || blocks_.at(bbi).full()) {
            // 如果最后一个块不存在或者已满，则需要创建新块并调整大小
            // 计算新块的大小：
            // - 如果当前块不存在，初始大小为 BASE_SIZE
            // - 如果当前块存在且已满，新块大小为原块大小的两倍
            c_size newSize = ifelse(bbi == BLOCK_NOT_EXISTS, BASE_SIZE, blocks_.at(bbi).size() << 1);
            ++backBlockIndex_;            // 将最后一个块的索引递增，指向新块
            back_block().resize(newSize); // 调整新块的大小
        }
    }

private:
    c_size size_;                   // 元素个数
    i32 backBlockIndex_;            // 最后一个块的索引
    Array<Buffer<value_t>> blocks_; // 动态块数组
};

/**
 * @brief 从动态数组中选择指定类型的参数。
 * @tparam T 目标类型。
 * @param args 动态数组。
 * @param index 参数索引。
 * @return 返回目标类型的参数值，如果类型不匹配或索引超出范围返回 None。
 */
template <typename T>
def opt(const DynArray<std::any>& args, c_size index)->T {
    if (index < 0 || index >= args.size()) {
        ValueError("Index out of range in opt function.");
        return None<T>;
    }

    try {
        return std::any_cast<T>(args.at(index));
    } catch (const std::bad_any_cast& e) {
        ValueError(std::format("Type mismatch in opt function: expected[{}], got[{}]", typeid(T).name(), args[index].type().name()));
        return None<T>;
    }
}

} // namespace my::util

#endif // DYN_ARRAY_HPP