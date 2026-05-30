/**
 * @brief 自动微分系统：计算图与梯度函数
 * @author Ricky
 * @date 2026/5/30
 * @version 1.0
 */
#ifndef AUTOGRAD_HPP
#define AUTOGRAD_HPP

#include "tensor.hpp"

namespace my::nn {

/**
 * @class GradFn
 * @brief 梯度函数基类
 * @tparam T 张量元素类型
 * @tparam Alloc 内存分配器
 *
 * 每个GradFn节点代表计算图中的一个操作，
 * 保存前向传播时的输入张量引用，用于反向传播时计算梯度。
 */
template <typename T, typename Alloc = mem::Allocator<T>>
class GradFn : public Object<GradFn<T, Alloc>> {
public:
    using TensorT = Tensor<T, Alloc>;

    virtual ~GradFn() = default;

    /**
     * @brief 反向传播
     * @param grad_output 上游传回的梯度
     *
     * 子类应实现此方法，计算对各输入的梯度，
     * 并通过 input._add_grad(gradient) 累加到输入张量的 grad_ 字段。
     */
    virtual void backward(const TensorT& grad_output) = 0;

    /**
     * @brief 转换为字符串
     * @return 字符串表示
     */
    [[nodiscard]] CString to_string() const {
        return CString{"GradFn"};
    }
};

// ==================== 具体梯度函数 ====================

/**
 * @brief 加法反向传播
 *
 * 存储原始输入张量的指针，反向传播时直接修改原张量的grad字段。
 */
template <typename T, typename Alloc = mem::Allocator<T>>
class AddBackward : public GradFn<T, Alloc> {
public:
    using TensorT = Tensor<T, Alloc>;

    AddBackward(const TensorT& self, const TensorT& other)
        : self_ptr_(&self), other_ptr_(&other) {}

    void backward(const TensorT& grad_output) override {
        self_ptr_->_add_grad(grad_output);
        other_ptr_->_add_grad(grad_output);
    }

    [[nodiscard]] CString to_string() const {
        return CString{"AddBackward"};
    }

private:
    const TensorT* self_ptr_;
    const TensorT* other_ptr_;
};

/**
 * @brief 减法反向传播
 */
template <typename T, typename Alloc = mem::Allocator<T>>
class SubBackward : public GradFn<T, Alloc> {
public:
    using TensorT = Tensor<T, Alloc>;

    SubBackward(const TensorT& self, const TensorT& other)
        : self_ptr_(&self), other_ptr_(&other) {}

    void backward(const TensorT& grad_output) override {
        self_ptr_->_add_grad(grad_output);
        other_ptr_->_add_grad(-grad_output);
    }

    [[nodiscard]] CString to_string() const {
        return CString{"SubBackward"};
    }

private:
    const TensorT* self_ptr_;
    const TensorT* other_ptr_;
};

/**
 * @brief 乘法反向传播
 */
template <typename T, typename Alloc = mem::Allocator<T>>
class MulBackward : public GradFn<T, Alloc> {
public:
    using TensorT = Tensor<T, Alloc>;

    MulBackward(const TensorT& self, const TensorT& other)
        : self_ptr_(&self), other_ptr_(&other) {}

    void backward(const TensorT& grad_output) override {
        self_ptr_->_add_grad(grad_output.broadcast_mul(*other_ptr_));
        other_ptr_->_add_grad(grad_output.broadcast_mul(*self_ptr_));
    }

    [[nodiscard]] CString to_string() const {
        return CString{"MulBackward"};
    }

private:
    const TensorT* self_ptr_;
    const TensorT* other_ptr_;
};

/**
 * @brief 除法反向传播
 */
template <typename T, typename Alloc = mem::Allocator<T>>
class DivBackward : public GradFn<T, Alloc> {
public:
    using TensorT = Tensor<T, Alloc>;

    DivBackward(const TensorT& self, const TensorT& other)
        : self_ptr_(&self), other_ptr_(&other) {}

    void backward(const TensorT& grad_output) override {
        TensorT inv_other = other_ptr_->broadcast_pow(-1);
        self_ptr_->_add_grad(grad_output.broadcast_mul(inv_other));
        other_ptr_->_add_grad(-grad_output.broadcast_mul(*self_ptr_).broadcast_mul(inv_other).broadcast_mul(inv_other));
    }

    [[nodiscard]] CString to_string() const {
        return CString{"DivBackward"};
    }

private:
    const TensorT* self_ptr_;
    const TensorT* other_ptr_;
};

/**
 * @brief 矩阵乘法反向传播
 */
template <typename T, typename Alloc = mem::Allocator<T>>
class MatMulBackward : public GradFn<T, Alloc> {
public:
    using TensorT = Tensor<T, Alloc>;

    MatMulBackward(const TensorT& self, const TensorT& other)
        : self_ptr_(&self), other_ptr_(&other) {}

    void backward(const TensorT& grad_output) override {
        // grad_self = grad_output @ other.T
        // grad_other = self.T @ grad_output
        const auto& self = *self_ptr_;
        const auto& other = *other_ptr_;
        usize b_ndim = other.ndim();
        usize g_ndim = grad_output.ndim();


        if (self.ndim() == 1 && other.ndim() == 1) {
            // 1D @ 1D: 标量结果
            self_ptr_->_add_grad(grad_output.broadcast_mul(other));
            other_ptr_->_add_grad(grad_output.broadcast_mul(self));
        } else if (self.ndim() == 1) {
            // 1D @ 2D: 向量×矩阵 → 1D
            auto g2d = grad_output.unsqueeze(0); // (1, m)
            auto o_t = other.transpose(b_ndim - 2, b_ndim - 1); // (p, n)
            self_ptr_->_add_grad(g2d.matmul(o_t).squeeze(0));
            // other grad: unsqueeze(0) -> matmul -> squeeze
            auto s1 = self.unsqueeze(0); // (1, n)
            auto g_unsq = grad_output.unsqueeze(1); // (m, 1)
            other_ptr_->_add_grad(s1.transpose(0, 1).matmul(g_unsq).squeeze(1));
        } else if (other.ndim() == 1) {
            // 2D @ 1D: 矩阵×向量 → 1D
            auto g2d = grad_output.unsqueeze(1); // (m, 1)
            other_ptr_->_add_grad(self.transpose(self.ndim() - 2, self.ndim() - 1).matmul(g2d).squeeze(1));
            auto o_unsq = other.unsqueeze(1); // (n, 1)
            self_ptr_->_add_grad(grad_output.unsqueeze(1).matmul(o_unsq.transpose(1, 0)));
        } else {
            // ND @ ND: 批量矩阵乘法
            TensorT self_t = self.transpose(self.ndim() - 2, self.ndim() - 1);
            TensorT other_t = other.transpose(other.ndim() - 2, other.ndim() - 1);
            self_ptr_->_add_grad(grad_output.matmul(other_t));
            other_ptr_->_add_grad(self_t.matmul(grad_output));
        }
    }

    [[nodiscard]] CString to_string() const {
        return CString{"MatMulBackward"};
    }

private:
    const TensorT* self_ptr_;
    const TensorT* other_ptr_;
};

/**
 * @brief 取负反向传播
 */
template <typename T, typename Alloc = mem::Allocator<T>>
class NegBackward : public GradFn<T, Alloc> {
public:
    using TensorT = Tensor<T, Alloc>;

    explicit NegBackward(const TensorT& self)
        : self_ptr_(&self) {}

    void backward(const TensorT& grad_output) override {
        self_ptr_->_add_grad(-grad_output);
    }

    [[nodiscard]] CString to_string() const {
        return CString{"NegBackward"};
    }

private:
    const TensorT* self_ptr_;
};

/**
 * @brief 求和反向传播
 */
template <typename T, typename Alloc = mem::Allocator<T>>
class SumBackward : public GradFn<T, Alloc> {
public:
    using TensorT = Tensor<T, Alloc>;

    explicit SumBackward(const TensorT& input)
        : input_shape_(input.shape()), input_ptr_(&input) {}

    void backward(const TensorT& grad_output) override {
        // 将标量梯度广播回输入形状
        TensorT grad = grad_output;
        while (grad.ndim() < input_shape_.len()) {
            grad = grad.unsqueeze(0);
        }
        // 沿输入形状各维广播
        for (usize d = 0; d < input_shape_.len(); ++d) {
            if (input_shape_[d] > 1 && (grad.shape()[d] == 1 || grad.ndim() <= d)) {
                grad = grad.broadcast_add(TensorT::zeros(input_shape_));
            }
        }
        // 直接使用broadcast_mul(1)实现广播
        TensorT ones(input_shape_, static_cast<T>(1));
        input_ptr_->_add_grad(grad.broadcast_mul(ones));
    }

    [[nodiscard]] CString to_string() const {
        return CString{"SumBackward"};
    }

private:
    typename TensorT::Shape input_shape_;
    const TensorT* input_ptr_;
};

/**
 * @brief 幂运算反向传播: y = x^exp → dx = exp * x^(exp-1) * dy
 */
template <typename T, typename Alloc = mem::Allocator<T>>
class PowBackward : public GradFn<T, Alloc> {
public:
    using TensorT = Tensor<T, Alloc>;

    PowBackward(const TensorT& self, T exp)
        : self_ptr_(&self), exp_(exp) {}

    void backward(const TensorT& grad_output) override {
        // dx = exp * x^(exp-1) * grad_output
        T exp_minus_1 = (exp_ == static_cast<T>(0)) ? static_cast<T>(0) : exp_ - static_cast<T>(1);
        TensorT grad = self_ptr_->broadcast_pow(exp_minus_1).broadcast_mul(
            TensorT::scalar(exp_)
        ).broadcast_mul(grad_output);
        self_ptr_->_add_grad(grad);
    }

    [[nodiscard]] CString to_string() const {
        return CString{"PowBackward"};
    }

private:
    const TensorT* self_ptr_;
    T exp_;
};

// ==================== 自动微分包装函数 ====================

/**
 * @brief 自动微分工具函数
 * @details 若任一输入需要梯度，创建对应的GradFn并附加到输出张量
 */

namespace detail {

template <typename T, typename Alloc, typename Fn, typename... Tensors>
Tensor<T, Alloc> _autograd_op(
    Tensor<T, Alloc>&& output,
    std::shared_ptr<GradFn<T, Alloc>> fn,
    const Tensor<T, Alloc>& first,
    const Tensors&... rest) {
    bool needs_grad = first.requires_grad() || (... || rest.requires_grad());
    if (needs_grad) {
        output._set_grad_fn(std::move(fn));
    }
    return output;
}

} // namespace detail

/**
 * @brief 自动微分加法
 */
template <typename T, typename Alloc = mem::Allocator<T>>
Tensor<T, Alloc> autograd_add(const Tensor<T, Alloc>& a, const Tensor<T, Alloc>& b) {
    auto result = a.broadcast_add(b);
    if (a.requires_grad() || b.requires_grad()) {
        result._set_grad_fn(std::make_shared<AddBackward<T, Alloc>>(a, b));
    }
    return result;
}

/**
 * @brief 自动微分减法
 */
template <typename T, typename Alloc = mem::Allocator<T>>
Tensor<T, Alloc> autograd_sub(const Tensor<T, Alloc>& a, const Tensor<T, Alloc>& b) {
    auto result = a.broadcast_sub(b);
    if (a.requires_grad() || b.requires_grad()) {
        result._set_grad_fn(std::make_shared<SubBackward<T, Alloc>>(a, b));
    }
    return result;
}

/**
 * @brief 自动微分乘法
 */
template <typename T, typename Alloc = mem::Allocator<T>>
Tensor<T, Alloc> autograd_mul(const Tensor<T, Alloc>& a, const Tensor<T, Alloc>& b) {
    auto result = a.broadcast_mul(b);
    if (a.requires_grad() || b.requires_grad()) {
        result._set_grad_fn(std::make_shared<MulBackward<T, Alloc>>(a, b));
    }
    return result;
}

/**
 * @brief 自动微分除法
 */
template <typename T, typename Alloc = mem::Allocator<T>>
Tensor<T, Alloc> autograd_div(const Tensor<T, Alloc>& a, const Tensor<T, Alloc>& b) {
    auto result = a.broadcast_div(b);
    if (a.requires_grad() || b.requires_grad()) {
        result._set_grad_fn(std::make_shared<DivBackward<T, Alloc>>(a, b));
    }
    return result;
}

/**
 * @brief 自动微分矩阵乘法
 */
template <typename T, typename Alloc = mem::Allocator<T>>
Tensor<T, Alloc> autograd_matmul(const Tensor<T, Alloc>& a, const Tensor<T, Alloc>& b) {
    auto result = a.matmul(b);
    if (a.requires_grad() || b.requires_grad()) {
        result._set_grad_fn(std::make_shared<MatMulBackward<T, Alloc>>(a, b));
    }
    return result;
}

/**
 * @brief 自动微分取负
 */
template <typename T, typename Alloc = mem::Allocator<T>>
Tensor<T, Alloc> autograd_neg(const Tensor<T, Alloc>& a) {
    auto result = a.neg();
    if (a.requires_grad()) {
        result._set_grad_fn(std::make_shared<NegBackward<T, Alloc>>(a));
    }
    return result;
}

/**
 * @brief 自动微分求和
 */
template <typename T, typename Alloc = mem::Allocator<T>>
Tensor<T, Alloc> autograd_sum(const Tensor<T, Alloc>& a) {
    auto result = a.sum_tensor();
    if (a.requires_grad()) {
        result._set_grad_fn(std::make_shared<SumBackward<T, Alloc>>(a));
    }
    return result;
}

} // namespace my::nn

#endif // AUTOGRAD_HPP
