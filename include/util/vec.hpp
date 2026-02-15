/**
 * @brief 动态数组容器
 * @author Ricky
 * @date 2025/2/28
 * @version 1.0
 */
#ifndef VEC_HPP
#define VEC_HPP

#include "array.hpp"

#include <any>

namespace my::util {

/**
 * @class Vec
 * @brief 动态数组容器
 * @tparam T 元素类型
 */
template <typename T, typename Alloc = mem::Allocator<T>>
class Vec : public Sequence<Vec<T>, T> {
public:
    using value_t = T;
    using Self = Vec<value_t, Alloc>;
    using Super = Sequence<Vec<value_t>, value_t>;

    /**
     * @brief 默认构造函数
     * @note 创建一个空向量，容量为0
     * @param alloc 内存分配器
     */
    Vec(const Alloc& alloc = Alloc{}) :
            alloc_(alloc), len_(0), capacity_(DEFAULT_CAPACITY), data_(alloc_.allocate(capacity_)) {}

    /**
     * @brief 构造指定大小的向量并用默认值填充
     * @param size 初始元素个数
     * @param val 用于填充的值，默认为值初始化
     * @param alloc 内存分配器
     */
    explicit Vec(const usize size, const value_t& val = value_t{}, const Alloc& alloc = Alloc{}) :
            alloc_(alloc), len_(size), capacity_(len_), data_(alloc_.allocate(capacity_)) {
        for (usize i = 0; i < len_; ++i) {
            alloc_.construct(data_ + i, val);
        }
    }

    /**
     * @brief 从初始化列表构造
     * @param init_list 初始化列表，元素将被拷贝
     * @param alloc 内存分配器
     */
    Vec(std::initializer_list<value_t>&& init_list, const Alloc& alloc = Alloc{}) :
            alloc_(alloc), len_(init_list.size()), capacity_(len_), data_(alloc_.allocate(capacity_)) {
        usize pos = 0;
        for (auto&& item : init_list) {
            alloc_.construct(data_ + pos, std::forward<decltype(item)>(item));
            pos++;
        }
    }

    /**
     * @brief 从可迭代对象构造
     * @tparam I 满足Iterable概念的类型
     * @param iter 可迭代对象，元素将被拷贝
     * @param alloc 内存分配器
     */
    template <Iterable I>
    Vec(I&& iter, const Alloc& alloc = Alloc{}) :
            alloc_(alloc), len_(iter.len()), capacity_(len_), data_(alloc_.allocate(capacity_)) {
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
            alloc_(other.alloc_), len_(other.len_), capacity_(other.capacity_), data_(alloc_.allocate(capacity_)) {
        for (usize i = 0; i < len_; ++i) {
            alloc_.construct(data_ + i, other.data_[i]);
        }
    }

    /**
     * @brief 移动构造函数
     * @param other 被移动的向量
     */
    Vec(Self&& other) noexcept :
            alloc_(other.alloc_), len_(other.len_), capacity_(other.capacity_), data_(other.data_) {
        other.data_ = nullptr;
        other.len_ = other.capacity_ = 0;
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
            alloc_.deallocate(data_, len_);
        }
        capacity_ = 0;
    }

    /**
     * @brief 获取容量
     * @return 容量
     */
    usize capacity() const {
        return capacity_;
    }

    /**
     * @brief 获取元素个数
     * @return 当前存储的元素个数
     */
    usize len() const noexcept {
        return len_;
    }

    /**
     * @brief 判断是否为空
     * @return true=是 false=否
     */
    bool is_empty() const noexcept {
        return len_ == 0;
    }

    /**
     * @brief 获取数据指针
     * @note 需要用户维护指针的安全性
     * @return 数据指针
     */
    value_t* data() {
        return data_;
    }

    /**
     * @brief 获取数据指针（常量版本）
     * @note 需要用户维护指针的安全性
     * @return 数据指针
     */
    const value_t* data() const {
        return data_;
    }

    /**
     * @brief 访问首元素
     * @note 空向量访问时行为未定义
     * @return 首元素的引用
     */
    value_t& first() noexcept {
        return data_[0];
    }

    /**
     * @brief 访问首元素（常量版本）
     * @note 空向量访问时行为未定义
     * @return 首元素的引用
     */
    const value_t& first() const noexcept {
        return data_[0];
    }

    /**
     * @brief 访问末元素
     * @note 空向量访问时行为未定义
     * @return 末元素的引用
     */
    value_t& last() noexcept {
        return data_[len_ - 1];
    }

    /**
     * @brief 访问末元素（常量版本）
     * @note 空向量访问时行为未定义
     * @return 末元素的引用
     */
    const value_t& last() const noexcept {
        return data_[len_ - 1];
    }

    /**
     * @brief 访问元素
     * @note 如果索引超出范围，行为未定义
     * @param idx 元素下标
     * @return 对应元素的引用
     */
    value_t& at(usize idx) {
        return data_[idx];
    }

    /**
     * @brief 访问元素（常量版本）
     * @note 如果索引超出范围，行为未定义
     * @param idx 元素下标
     * @return 对应元素的引用
     */
    const value_t& at(usize idx) const {
        return data_[idx];
    }

    /**
     * @brief 线性查找元素
     * @details 时间复杂度 O(n)
     * @param value 目标值
     * @return 若找到返回索引，否则返回向量长度
     */
    usize find(const value_t& value) const {
        for (usize i = 0; i < len_; ++i) {
            if (data_[i] == value) {
                return i;
            }
        }
        return len_;
    }

    /**
     * @brief 追加元素到向量末尾
     * @param item 要追加的元素
     * @return 被追加元素的引用
     */
    template <typename U>
    value_t& push(U&& item) {
        try_expand();
        alloc_.construct(data_ + len_, std::forward<U>(item));
        ++len_;
        return last();
    }

    /**
     * @brief 原地构造追加元素到向量末尾
     * @param args 构造元素的参数
     */
    template <typename... Args>
    value_t& push(Args&&... args) {
        try_expand();
        alloc_.construct(data_ + len_, std::forward<Args>(args)...);
        ++len_;
        return last();
    }

    /**
     * @brief 在指定位置插入元素
     * @tparam Args 可转发类型
     * @param idx 插入位置，从0开始
     * @param args 要插入的元素
     */
    template <typename... Args>
    void insert(usize idx, Args&&... args) {
        if (idx > len_) return;
        try_expand();
        for (auto it = Super::end(); it >= Super::begin() + idx; --it) {
            *std::next(it) = std::move(*it);
        }
        alloc_.construct(data_ + idx, std::forward<Args>(args)...);
        ++len_;
    }

    /**
     * @brief 移除指定位置的元素
     * @param idx 移除位置，从 0 开始，默认移除最后一个元素
     */
    void pop(isize idx = -1) {
        if (is_empty()) return;

        idx = neg_index(idx, static_cast<isize>(len_));
        for (auto it = Super::begin() + idx + 1; it != Super::end(); ++it) {
            *std::prev(it) = std::move(*it);
        }
        alloc_.destroy(data_ + idx);
        --len_;
    }

    /**
     * @brief 清空所有元素，容量不变
     */
    void clear() {
        alloc_.destroy_n(data_, len_);
        len_ = 0;
    }

    /**
     * @brief 交换两个向量内容
     * @param other 另一个向量
     */
    void swap(Self& other) noexcept {
        std::swap(alloc_, other.alloc_);
        std::swap(capacity_, other.capacity_);
        std::swap(len_, other.len_);
        std::swap(data_, other.data_);
    }

    /**
     * @brief 转换为 Array，复制元素
     * @return 返回包含所有元素的 Array
     */
    Array<value_t> to_array() const {
        Array<value_t> arr(len_);
        for (usize i = 0; i < len_; ++i) {
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
        Array<value_t> arr(len_);
        for (usize i = 0; i < len_; ++i) {
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
        const auto m_size = len();
        start = neg_index(start, m_size);
        usize u_end = static_cast<usize>(neg_index(end, static_cast<isize>(m_size)));
        Self ans(u_end - start);
        usize pos = 0;
        for (usize i = start; i < u_end; ++i) {
            ans.at(pos++) = at(i);
        }
        return ans;
    }

    /**
     * @brief 向量切片，返回从指定索引开始到末尾的子数组
     * @param start 起始索引
     * @return 子数组
     */
    Self slice(const usize start) const {
        return slice(start, len());
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
            push(std::forward<decltype(item)>(item));
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
        const usize min_size = std::min(len_, new_cap);

        if constexpr (std::is_trivially_copyable_v<value_t>) {
            std::memcpy(ptr, data_, min_size * sizeof(value_t));
        } else {
            for (usize i = 0; i < min_size; ++i) {
                new (ptr + i) value_t(std::move(data_[i]));
                data_[i].~value_t();
            }
        }

        alloc_.deallocate(data_, len_);
        data_ = ptr;
        capacity_ = new_cap;
    }

    /**
     * @brief 分离数组，返回数组指针和大小，并将数组置空
     * @return 返回包含数组大小和指针的 Pair
     * @note 分离后，原数组将不再管理数组的内存，用户需要手动管理返回的指针
     */
    Pair<usize, value_t*> separate() {
        auto res = Pair{len_, data_};
        data_ = nullptr;
        len_ = capacity_ = 0;
        return res;
    }

    /**
     * @brief 预留存储空间
     * @param new_cap 新的容量
     */
    void reserve(const usize new_cap) {
        if (new_cap > capacity_) {
            resize(std::max(new_cap, capacity_ << 1));
        }
    }

    /**
     * @brief 获取动态数组的字符串表示
     * @return 返回动态数组的 CSV 格式的字符串
     */
    [[nodiscard]] CString __str__() const {
        std::stringstream stream;
        stream << '[';
        for (usize i = 0; i < len_; ++i) {
            if (i) stream << ',';
            stream << at(i);
        }
        stream << ']';
        return CString{stream.str()};
    }

private:
    auto try_expand() {
        if (len_ == capacity_) {
            resize(capacity_ << 1);
        }
    }

private:
    Alloc alloc_{};  // 内存分配器
    usize len_;     // 元素个数
    usize capacity_; // 总容量
    value_t* data_;  // 指向数据首地址的指针

    static constexpr usize DEFAULT_CAPACITY = 16;
};

/**
 * @brief 从动态数组中选择指定类型的参数
 * @tparam T 目标类型
 * @param args 动态数组
 * @param idx 参数索引
 * @return 返回目标类型的参数值
 * @exception Exception 若索引超出范围，则抛出 index_out_of_bounds_exception
 * @exception Exception 若类型不匹配，则抛出 type_exception
 */
template <typename T>
auto opt(const Vec<std::any>& args, usize idx) -> T {
    if (idx >= args.len()) {
        throw index_out_of_bounds_exception("Index {} out of bounds [0..{}] in opt function.", idx, args.len());
    }

    try {
        return std::any_cast<T>(args.at(idx));
    } catch (const std::bad_any_cast& e) {
        throw type_exception("type mismatch in opt function: expected[{}], got[{}]", dtype(T), args[idx].type().name());
    }
}

/**
 * @brief 判断是否为Vec类型
 */
template <typename>
struct is_vec : std::false_type {};

template <typename T>
struct is_vec<Vec<T>> : std::true_type {};

template <typename T>
constexpr bool is_vec_v = is_vec<T>::value;

} // namespace my::util

#endif // VEC_HPP