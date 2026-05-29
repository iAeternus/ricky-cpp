/**
 * @brief 张量，N维数组
 * @author Ricky
 * @date 2026/5/29
 * @version 1.0
 */
#ifndef TENSOR_HPP
#define TENSOR_HPP

#include "vec.hpp"
#include "math_concepts.hpp"

namespace my::ai {

template <typename T, typename Alloc = mem::Allocator<T>>
class Tensor : public Object<Tensor<T, Alloc>> {
public:
    using Self = Tensor<T, Alloc>;
    using UsizeAlloc = typename Alloc::template rebind<usize>::other;
    using Storage = util::Vec<T, Alloc>;
    using Shape = util::Vec<usize, UsizeAlloc>;

    /**
     * @brief 构造空张量
     */
    Tensor() = default;

    /**
     * @brief 构造连续张量
     * @param shape 张量形状
     */
    explicit Tensor(const Shape& shape) :
            data_(std::make_shared<Storage>(calc_numel(shape))),
            shape_(shape),
            stride_(make_stride(shape_)) {}

    /**
     * @brief 构造连续张量
     * @param shape 张量形状
     */
    explicit Tensor(Shape&& shape) :
            data_(std::make_shared<Storage>(calc_numel(shape))),
            shape_(std::move(shape)),
            stride_(make_stride(shape_)) {}

    /**
     * @brief 使用指定值填充张量
     * @param shape 张量形状
     * @param value 初始值
     */
    Tensor(const Shape& shape, const T& value) :
            data_(std::make_shared<Storage>(calc_numel(shape), value)),
            shape_(shape),
            stride_(make_stride(shape_)) {}

    /**
     * @brief 创建全0张量
     * @param shape 张量形状
     * @return 张量
     */
    static Self zeros(const Shape& shape) {
        return Tensor(shape, static_cast<T>(0));
    }

    /**
     * @brief 创建全1张量
     * @param shape 张量形状
     * @return 张量
     */
    static Self ones(const Shape& shape) {
        return Tensor(shape, static_cast<T>(1));
    }

    /**
     * @brief 创建随机张量
     * @param shape 张量形状
     * @return 张量
     */
    static Self rand(const Shape& shape);

    /**
     * @brief 创建正态分布随机张量
     * @param shape 张量形状
     * @param mean 均值
     * @param stddev 标准差
     * @return 张量
     */
    static Self randn(const Shape& shape, T mean = static_cast<T>(0), T stddev = static_cast<T>(1));

    /**
     * @brief 创建等差张量
     * @param start 起始值
     * @param end 终止值
     * @param step 步长
     * @return 张量
     */
    static Self arange(T start, T end, T step = static_cast<T>(1));

    /**
     * @brief 获取维度数
     * @return 维度数
     */
    [[nodiscard]] usize ndim() const noexcept;

    /**
     * @brief 获取元素个数
     * @return 元素个数
     */
    [[nodiscard]] usize numel() const noexcept;

    /**
     * @brief 获取张量形状
     * @return 张量形状
     */
    [[nodiscard]] const Shape& shape() const noexcept;

    /**
     * @brief 获取步长
     * @return 步长
     */
    [[nodiscard]] const Shape& stride() const noexcept;

    /**
     * @brief 获取偏移量
     * @return 偏移量
     */
    [[nodiscard]] usize offset() const noexcept;

    /**
     * @brief 判断是否为空张量
     * @return 是否为空
     */
    [[nodiscard]] bool is_empty() const noexcept;

    /**
     * @brief 判断是否为连续内存
     * @return 是否连续
     */
    [[nodiscard]] bool is_contiguous() const noexcept;

    /**
     * @brief 判断是否为view张量
     * @return 是否为view
     */
    [[nodiscard]] bool is_view() const noexcept;

    /**
     * @brief 获取底层数据指针
     * @return 数据指针
     */
    [[nodiscard]] T* data() noexcept;

    /**
     * @brief 获取底层数据指针
     * @return 数据指针
     */
    [[nodiscard]] const T* data() const noexcept;

    /**
     * @brief 多维索引访问
     * @param indices 多维坐标
     * @return 元素引用
     */
    template <math::IntegerType... Args>
    T& operator()(Args&&... indices);

    /**
     * @brief 多维索引访问
     * @param indices 多维坐标
     * @return 元素引用
     */
    template <math::IntegerType... Args>
    const T& operator()(Args&&... indices) const;

    /**
     * @brief reshape张量
     * @param shape 新形状
     * @return view张量
     *
     * 不复制数据。
     */
    [[nodiscard]]
    Self view(const Shape& shape) const;

    /**
     * @brief reshape张量
     * @param shape 新形状
     * @return view张量
     *
     * 若内存不连续则复制数据。
     */
    [[nodiscard]]
    Self reshape(const Shape& shape) const;

    /**
     * @brief 转置两个维度
     * @param dim0 第一个维度
     * @param dim1 第二个维度
     * @return view张量
     */
    [[nodiscard]] Self transpose(usize dim0, usize dim1) const;

    /**
     * @brief 重排维度
     * @param dims 新维度顺序
     * @return view张量
     */
    [[nodiscard]] Self permute(const Shape& dims) const;

    /**
     * @brief 获取切片
     * @param dim 维度
     * @param start 起始下标
     * @param end 结束下标，不包含
     * @return view张量
     */
    [[nodiscard]] Self slice(usize dim, usize start, usize end) const;

    /**
     * @brief 展平张量
     * @return view张量
     */
    [[nodiscard]] Self flatten() const;

    /**
     * @brief 插入长度为1的维度
     * @param dim 插入位置
     * @return view张量
     */
    [[nodiscard]] Self unsqueeze(usize dim) const;

    /**
     * @brief 删除长度为1的维度
     * @param dim 删除位置
     * @return view张量
     */
    [[nodiscard]] Self squeeze(usize dim) const;

    /**
     * @brief 转换为连续内存
     * @return 连续张量
     */
    [[nodiscard]] Self contiguous() const;

    /**
     * @brief 张量加法
     * @param rhs 右操作数
     * @return 结果张量
     */
    [[nodiscard]] Self operator+(const Self& rhs) const;

    /**
     * @brief 张量减法
     * @param rhs 右操作数
     * @return 结果张量
     */
    [[nodiscard]] Self operator-(const Self& rhs) const;

    /**
     * @brief 张量乘法
     * @param rhs 右操作数
     * @return 结果张量
     */
    [[nodiscard]] Self operator*(const Self& rhs) const;

    /**
     * @brief 张量除法
     * @param rhs 右操作数
     * @return 结果张量
     */
    [[nodiscard]] Self operator/(const Self& rhs) const;

    /**
     * @brief 矩阵乘法
     * @param rhs 右操作数
     * @return 结果张量
     */
    [[nodiscard]] Self matmul(const Self& rhs) const;

    /**
     * @brief 求和
     * @return 标量
     */
    [[nodiscard]] T sum() const;

    /**
     * @brief 求均值
     * @return 标量
     */
    [[nodiscard]] T mean() const;

    /**
     * @brief 求最大值
     * @return 标量
     */
    [[nodiscard]] T max() const;

    /**
     * @brief 求最小值
     * @return 标量
     */
    [[nodiscard]] T min() const;

private:
    /**
     * @brief 计算连续内存步长
     * @param shape 张量形状
     * @return 步长
     *
     * 例如：
     *
     * shape:
     * [2,3,4]
     *
     * stride:
     * [12,4,1]
     */
    [[nodiscard]] static Shape make_stride(const Shape& shape) {
        Shape stride(shape.len());
        usize acc = 1;
        for (isize i = static_cast<isize>(shape.len()) - 1; i >= 0; --i) {
            stride[i] = acc;
            acc *= shape[i];
        }
        return stride;
    }

    /**
     * @brief 计算元素个数
     * @param shape 张量形状
     * @return 元素个数
     */
    [[nodiscard]] static usize calc_numel(const Shape& shape) {
        if (shape.is_empty()) {
            return 1; // 空shape视为标量
        }

        usize numel = 1;
        for (usize dim : shape) {
            numel *= dim;
        }
        return numel;
    }

    /**
     * @brief 计算真实内存下标
     * @param indices 多维坐标
     * @return 一维下标
     *
     * 公式：
     *
     * offset + sum(indices[i] * stride[i])
     */
    template <math::UnsignedIntegerType... Args>
    [[nodiscard]] usize calc_offset(Args&&... indices) const {
        static_assert(sizeof...(Args) > 0);
        if (sizeof...(Args) == shape_.size()) {
            throw tensor_exception("Tensor indices dimension mismatch");
        }

        usize idx[] = {static_cast<usize>(indices)...};
        usize off = offset_;
        usize shape_len = shape_.len();
        for (usize i = 0; i < shape_len; ++i) {
            if (idx[i] >= shape_[i]) {
                throw tensor_exception("Tensor index out of range");
            }
            off += idx[i] * stride_[i];
        }
        return off;
    }

private:
    std::shared_ptr<Storage> data_;
    Shape shape_;
    Shape stride_;
    usize offset_ = 0;
};

} // namespace my::ai

#endif // TENSOR_HPP
