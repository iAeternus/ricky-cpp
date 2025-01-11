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

namespace my::util {

template <typename T>
class DynArray : public Sequence<DynArray<T>, T> {
    using self = DynArray<T>;
    using super = Sequence<DynArray<T>, T>;
    // 动态数组块的数量
    constexpr static i32 DYNARRAY_BLOCK_SIZE = 63;
    // 最小块大小
    constexpr static c_size BASE_SIZE = 8ll;
    // 不存在块
    constexpr static i32 BLOCK_NOT_EXISTS = -1;

public:
    using value_t = T;

    DynArray() :
            size_(0), backBlockIndex_(BLOCK_NOT_EXISTS), blocks_(DYNARRAY_BLOCK_SIZE, 0) {}

    DynArray(std::initializer_list<value_t>&& init) :
            DynArray() {
        for (auto& item : init) {
            append(std::forward<decltype(item)>(item));
        }
    }

    template <Iterable I>
    DynArray(I&& other) :
            DynArray() {
        for (auto&& item : other) {
            append(std::forward<decltype(item)>(item));
        }
    }

    DynArray(const self& other) :
            DynArray() {
        for (const auto& item : other) {
            append(item);
        }
    }

    DynArray(self&& other) noexcept :
            size_(other.size_), backBlockIndex_(other.backBlockIndex_), blocks_(std::move(other.blocks_)) {
        other.size_ = 0;
        other.backBlockIndex_ = BLOCK_NOT_EXISTS;
    }

    self& operator=(const self& other) {
        if (this == &other) return *this;

        this->size_ = other.size_;
        this->backBlockIndex_ = other.backBlockIndex_;
        this->blocks_ = other.blocks_;
        return *this;
    }

    self& operator=(self&& other) noexcept {
        if (this == &other) return *this;

        this->size_ = other.size_;
        this->backBlockIndex_ = other.backBlockIndex_;
        this->blocks_ = std::move(other.blocks_);

        other.size_ = 0;
        other.backBlockIndex_ = BLOCK_NOT_EXISTS;

        return *this;
    }

    ~DynArray() = default;

    c_size size() const {
        return size_;
    }

    bool empty() const {
        return size_ == 0;
    }

    /**
     * @brief 对index范围不做检查
     */
    value_t& at(c_size index) {
        i32 blockIndex = get_block_index(index + 1);
        c_size inblockIndex = get_inblock_index(index + 1, blockIndex);
        return blocks_.at(blockIndex).at(inblockIndex);
    }

    /**
     * @brief 对index范围不做检查
     */
    const value_t& at(c_size index) const {
        i32 blockIndex = get_block_index(index + 1);
        c_size inblockIndex = get_inblock_index(index + 1, blockIndex);
        return blocks_.at(blockIndex).at(inblockIndex);
    }

    /**
     * @brief 线性查找，如果不存在返回长度
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
     * @brief 在动态数组末尾追加元素
     * @param item 需要追加的元素
     * @return 返回追加的元素
     */
    template <typename U>
    value_t& append(U&& item) {
        try_wakeup();
        auto& res = back_block().append(std::forward<U>(item));
        ++size_;
        return res;
    }

    template <typename U>
    void insert(c_size index, U&& item) {
        append(std::forward<U>(item));
        for (c_size i = size() - 1; i > index; --i) {
            std::swap(at(i), at(i - 1));
        }
    }

    /**
     * @brief 移除指定位置的元素
     * @param index 索引，从0开始。若为-1，则移除最后一个元素
     * @return void
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
     * @brief 转换为Array，拷贝
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
     * @brief 转换为Array，移动
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
     * @brief 将一个可迭代对象追加到动态数组的末尾
     * @param 可迭代对象
     * @return self&
     */
    template <Iterable I>
    self& extend(I&& other) {
        for (auto&& item : other) {
            append(std::forward<decltype(item)>(item));
        }
        return *this;
    }

    self operator+(const self& other) {
        return self(*this).extend(other);
    }

    self operator+(self&& other) {
        return self(*this).extend(std::forward<self>(other));
    }

    self& operator+=(const self& other) {
        return extend(other);
    }

    self& operator+=(self&& other) {
        return extend(std::forward<self>(other));
    }

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

        Iterator(container_t* dynarray = nullptr, c_size blockIndex = 0, c_size inblockIndex = 0) :
                blockIndex_(blockIndex), inblockIndex_(inblockIndex), dynarray_(dynarray) {}

        Iterator(const self& other) :
                Iterator(other.dynarray_, other.blockIndex_, other.inblockIndex_) {}

        self& operator=(const self& other) {
            this->blockIndex_ = other.blockIndex_;
            this->inblockIndex_ = other.inblockIndex_;
            this->dynarray_ = other.dynarray_;
            return *this;
        }

        reference operator*() {
            return dynarray_->blocks_.at(blockIndex_).at(inblockIndex_);
        }

        const_reference operator*() const {
            return dynarray_->blocks_.at(blockIndex_).at(inblockIndex_);
        }

        pointer operator->() {
            return &dynarray_->blocks_.at(blockIndex_).at(inblockIndex_);
        }

        const_pointer operator->() const {
            return &dynarray_->blocks_.at(blockIndex_).at(inblockIndex_);
        }

        self& operator++() {
            if (inblockIndex_ == dynarray_->blocks_.at(blockIndex_).size() - 1) {
                ++blockIndex_;
                inblockIndex_ = 0;
                return *this;
            }
            ++inblockIndex_;
            return *this;
        }

        self operator++(int) {
            self tmp(*this);
            ++tmp;
            return tmp;
        }

        self& operator--() {
            if (dynarray_->blocks_.at(blockIndex_).size() == 0) {
                --blockIndex_;
                inblockIndex_ = dynarray_->blocks_.at(blockIndex_).size() - 1;
                return *this;
            }
            --inblockIndex_;
            return *this;
        }

        self operator--(int) {
            self tmp(*this);
            --tmp;
            return tmp;
        }

        self& operator+=(difference_type n) { // TODO
            while (n > 0) {
                c_size curSize = dynarray_->blocks_.at(blockIndex_).size();
                if (inblockIndex_ + n < curSize) {
                    inblockIndex_ += n;
                    break;
                }
                n -= curSize - inblockIndex_;
                ++blockIndex_;
                inblockIndex_ = 0;
            }
            return *this;
        }

        self operator+(difference_type n) const {
            self tmp(*this);
            tmp += n;
            return tmp;
        }

        self& operator-=(difference_type n) {
            while (n > 0) {
                if (inblockIndex_ >= n) {
                    inblockIndex_ -= n;
                    break;
                }
                n -= inblockIndex_;
                --blockIndex_;
                inblockIndex_ = dynarray_->blocks_.at(blockIndex_).size() - 1;
            }
            return *this;
        }

        self operator-(difference_type n) const {
            self tmp(*this);
            tmp -= n;
            return tmp;
        }

        /**
         * @brief 计算this与other的差值
         */
        difference_type operator-(const self& other) const {
            if (__cmp__(*this, other) < 0) return -(other - *this);
            if (this->dynarray_ != other.dynarray_) {
                RuntimeError("Iterator not belong to the same container.");
            }

            if (this->blockIndex_ == other.blockIndex_) {
                return this->inblockIndex_ - other.inblockIndex_;
            }
            difference_type diff = this->dynarray_->blocks_.at(other.blockIndex_).size() - other.inblockIndex_;
            for (c_size i = other.blockIndex_ + 1; i < this->blockIndex_; ++i) {
                diff += this->dynarray_.at(i).size();
            }
            return diff + this->inblockIndex_;
        }

        bool operator==(const self& other) const {
            return __equals__(other);
        }

        bool operator!=(const self& other) const {
            return !__equals__(other);
        }

        bool __equals__(const self& other) const {
            return this->blockIndex_ == other.blockIndex_ && this->inblockIndex_ == other.inblockIndex_ && this->dynarray_ == other.dynarray_;
        }

        cmp_t __cmp__(const self& other) const {
            if (this->blockIndex_ != other.blockIndex_) {
                return this->blockIndex_ - other.blockIndex_;
            }
            return this->inblockIndex_ - other.inblockIndex_;
        }

    private:
        c_size blockIndex_, inblockIndex_;
        container_t* dynarray_;
    };

    using iterator = Iterator<false>;
    using const_iterator = Iterator<true>;

    iterator begin() {
        return iterator(this);
    }

    const_iterator begin() const {
        return const_iterator(this);
    }

    iterator end() {
        return iterator(this, back_block_index() + 1);
    }

    const_iterator end() const {
        return const_iterator(this, back_block_index() + 1);
    }

private:
    /**
     * @brief 得到第ith个元素的块索引，i从1开始，时间复杂度O(log n)
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
     * @brief 得到第ith个元素的块内索引，i从1开始
     */
    c_size get_inblock_index(c_size ith, i32 blockIndex) const {
        return ith - BASE_SIZE * (exp2[blockIndex] - 1) - 1;
    }

    /**
     * @brief 获取最后一个块的索引
     * @return 不存在块时返回-1
     */
    i32 back_block_index() const {
        return backBlockIndex_;
    }

    /**
     * @brief 获取最后一个块，不会做块的存在性检查
     */
    Buffer<T>& back_block() {
        return blocks_.at(back_block_index());
    }

    /**
     * @brief 移除最后一个块
     */
    void pop_back_block() {
        back_block().resize(0);
        --backBlockIndex_;
    }

    /**
     * @brief 尝试唤醒一个新的块，不会做Array的扩容
     */
    void try_wakeup() {
        i32 bbi = back_block_index();
        if (bbi == BLOCK_NOT_EXISTS || blocks_.at(bbi).full()) {
            c_size newSize = ifelse(bbi == BLOCK_NOT_EXISTS, BASE_SIZE, blocks_.at(bbi).size() << 1);
            ++backBlockIndex_;
            back_block().resize(newSize);
        }
    }

private:
    c_size size_;                   // 元素个数
    i32 backBlockIndex_;            // 最后一个块的索引
    Array<Buffer<value_t>> blocks_; // 动态块数组
};

} // namespace my::util

#endif // DYN_ARRAY_HPP