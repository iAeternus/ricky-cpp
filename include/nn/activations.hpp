/**
 * @brief 激活函数模块
 * @author Ricky
 * @date 2026/5/30
 * @version 1.0
 */
#ifndef ACTIVATIONS_HPP
#define ACTIVATIONS_HPP

#include "module.hpp"
#include "autograd.hpp"

namespace my::nn {

// ==================== ReLU 反向传播 ====================

/**
 * @brief ReLU 反向传播
 */
template <typename T, typename Alloc = mem::Allocator<T>>
class ReluBackward : public GradFn<T, Alloc> {
public:
    using TensorT = Tensor<T, Alloc>;

    explicit ReluBackward(const TensorT& input)
        : input_ptr_(&input) {}

    void backward(const TensorT& grad_output) override {
        // dx = dy * (x > 0)
        const auto& input_ = *input_ptr_;
        auto mask = input_.contiguous();
        TensorT grad(input_.shape());
        for (usize i = 0; i < mask.numel(); ++i) {
            grad.data()[i] = (mask.data()[i] > static_cast<T>(0))
                ? static_cast<T>(1) : static_cast<T>(0);
        }
        input_ptr_->_add_grad(grad_output.broadcast_mul(grad));
    }

    [[nodiscard]] CString to_string() const {
        return CString{"ReluBackward"};
    }

private:
    const TensorT* input_ptr_;
};

/**
 * @class ReLU
 * @brief ReLU 激活函数: ReLU(x) = max(0, x)
 */
template <typename T, typename Alloc = mem::Allocator<T>>
class ReLU : public Module<T, Alloc> {
public:
    using TensorT = Tensor<T, Alloc>;

    /**
     * @brief 构造 ReLU 层
     * @param inplace 是否原地操作（暂不支持）
     */
    explicit ReLU(bool inplace = false) : inplace_(inplace) {}

    /**
     * @brief 前向传播: ReLU(x) = max(0, x)
     *
     * 缓存输入到成员变量，确保 GradFn 中存储的原始指针在 backward() 完成前有效。
     */
    TensorT forward(const TensorT& input) override {
        if (inplace_) {
            throw tensor_exception("ReLU inplace not supported yet");
        }

        TensorT result(input.shape());
        auto inp = input.contiguous();
        for (usize i = 0; i < inp.numel(); ++i) {
            result.data()[i] = std::max(inp.data()[i], static_cast<T>(0));
        }

        cached_input_ = input;
        if (input.requires_grad()) {
            result._set_grad_fn(std::make_shared<ReluBackward<T, Alloc>>(cached_input_));
        }
        return result;
    }

    /** @brief 是否原地操作 */
    bool inplace_ = false;

private:
    TensorT cached_input_;
};

// ==================== Sigmoid 反向传播 ====================

/**
 * @brief Sigmoid 反向传播
 * dsigmoid/dx = sigmoid(x) * (1 - sigmoid(x))
 */
template <typename T, typename Alloc = mem::Allocator<T>>
class SigmoidBackward : public GradFn<T, Alloc> {
public:
    using TensorT = Tensor<T, Alloc>;

    SigmoidBackward(const TensorT& input, const TensorT& output)
        : input_ptr_(&input), output_ptr_(&output) {}

    void backward(const TensorT& grad_output) override {
        // dx = dy * y * (1 - y)
        auto one = TensorT::scalar(static_cast<T>(1));
        input_ptr_->_add_grad(
            grad_output.broadcast_mul(*output_ptr_).broadcast_mul(one.broadcast_sub(*output_ptr_))
        );
    }

    [[nodiscard]] CString to_string() const {
        return CString{"SigmoidBackward"};
    }

private:
    const TensorT* input_ptr_;
    const TensorT* output_ptr_;
};

/**
 * @class Sigmoid
 * @brief Sigmoid 激活函数: Sigmoid(x) = 1 / (1 + e^{-x})
 */
template <typename T, typename Alloc = mem::Allocator<T>>
class Sigmoid : public Module<T, Alloc> {
public:
    using TensorT = Tensor<T, Alloc>;

    /**
     * @brief 前向传播: Sigmoid(x) = 1 / (1 + exp(-x))
     *
     * 对正值使用 1/(1+exp(-x))，对负值使用 exp(x)/(1+exp(x)) 避免溢出。
     * 缓存输入和输出到成员变量，确保 GradFn 指针有效。
     */
    TensorT forward(const TensorT& input) override {
        TensorT result(input.shape());
        auto inp = input.contiguous();
        for (usize i = 0; i < inp.numel(); ++i) {
            T x = inp.data()[i];
            if (x >= static_cast<T>(0)) {
                result.data()[i] = static_cast<T>(1) / (static_cast<T>(1) + std::exp(-x));
            } else {
                T ex = std::exp(x);
                result.data()[i] = ex / (static_cast<T>(1) + ex);
            }
        }

        cached_input_ = input;
        cached_output_ = result;
        if (input.requires_grad()) {
            result._set_grad_fn(std::make_shared<SigmoidBackward<T, Alloc>>(cached_input_, cached_output_));
        }
        return result;
    }

private:
    TensorT cached_input_;
    TensorT cached_output_;
};

// ==================== Softmax 反向传播 ====================

/**
 * @brief Softmax 反向传播
 */
template <typename T, typename Alloc = mem::Allocator<T>>
class SoftmaxBackward : public GradFn<T, Alloc> {
public:
    using TensorT = Tensor<T, Alloc>;

    SoftmaxBackward(const TensorT& input, const TensorT& output, isize dim)
        : input_ptr_(&input), output_ptr_(&output), dim_(dim) {}

    void backward(const TensorT& grad_output) override {
        // dx = y * (dy - sum(y * dy, dim))
        // 其中 y = softmax(x)
        TensorT sdy = output_ptr_->broadcast_mul(grad_output);
        // 沿 dim 求和（简化：对整个张量求和后广播）
        TensorT sum_sdy = TensorT::scalar(sdy.sum());
        TensorT grad = output_ptr_->broadcast_mul(grad_output.broadcast_sub(sum_sdy));
        input_ptr_->_add_grad(grad);
    }

    [[nodiscard]] CString to_string() const {
        return CString{"SoftmaxBackward"};
    }

private:
    const TensorT* input_ptr_;
    const TensorT* output_ptr_;
    isize dim_;
};

/**
 * @class Softmax
 * @brief Softmax 激活函数: softmax(x_i) = exp(x_i) / sum(exp(x_j))
 *
 * 沿指定维度计算，默认最后一维。
 * 使用 max-subtraction 技巧保证数值稳定性。
 */
template <typename T, typename Alloc = mem::Allocator<T>>
class Softmax : public Module<T, Alloc> {
public:
    using TensorT = Tensor<T, Alloc>;
    using Shape = typename TensorT::Shape;

    /**
     * @brief 构造 Softmax
     * @param dim 计算维度，默认 -1（最后一维）
     */
    explicit Softmax(isize dim = -1) : dim_(dim) {}

    /**
     * @brief 前向传播
     * @param input 输入张量
     * @return softmax 后的张量
     *
     * 缓存输入和输出到成员变量，确保 GradFn 指针有效。
     */
    TensorT forward(const TensorT& input) override {
        isize actual_dim = dim_;
        if (actual_dim < 0) {
            actual_dim += static_cast<isize>(input.ndim());
        }

        T max_val = input.max();
        TensorT shifted = input.broadcast_sub(TensorT::scalar(max_val));

        TensorT exp_result(shifted.shape());
        auto inp = shifted.contiguous();
        for (usize i = 0; i < inp.numel(); ++i) {
            exp_result.data()[i] = std::exp(inp.data()[i]);
        }

        T sum_exp = exp_result.sum();

        TensorT result(exp_result.shape());
        for (usize i = 0; i < exp_result.numel(); ++i) {
            result.data()[i] = exp_result.data()[i] / sum_exp;
        }

        cached_input_ = input;
        cached_output_ = result;
        if (input.requires_grad()) {
            result._set_grad_fn(
                std::make_shared<SoftmaxBackward<T, Alloc>>(cached_input_, cached_output_, actual_dim)
            );
        }
        return result;
    }

    /** @brief 计算维度 */
    isize dim_;

private:
    TensorT cached_input_;
    TensorT cached_output_;
};

} // namespace my::nn

#endif // ACTIVATIONS_HPP
