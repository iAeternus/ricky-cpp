/**
 * @brief 损失函数模块
 * @author Ricky
 * @date 2026/5/30
 * @version 1.0
 */
#ifndef LOSS_HPP
#define LOSS_HPP

#include "autograd.hpp"

namespace my::nn {

// ==================== MSELoss 反向传播 ====================

/**
 * @brief MSELoss 反向传播
 */
template <typename T, typename Alloc = mem::Allocator<T>>
class MSELossBackward : public GradFn<T, Alloc> {
public:
    using TensorT = Tensor<T, Alloc>;

    MSELossBackward(const TensorT& input, const TensorT& target, const CString& reduction)
        : input_ptr_(&input), target_ptr_(&target), reduction_(reduction) {}

    void backward(const TensorT& grad_output) override {
        TensorT diff = input_ptr_->broadcast_sub(*target_ptr_);
        TensorT grad = diff.broadcast_mul(TensorT::scalar(static_cast<T>(2)));

        if (reduction_ == "mean") {
            grad = grad.broadcast_div(TensorT::scalar(static_cast<T>(input_ptr_->numel())));
        }

        // grad_output 是标量（上游梯度），广播到 diff 形状
        input_ptr_->_add_grad(grad.broadcast_mul(grad_output));
    }

    [[nodiscard]] CString to_string() const {
        return CString{"MSELossBackward"};
    }

private:
    const TensorT* input_ptr_;
    const TensorT* target_ptr_;
    CString reduction_;
};

/**
 * @class MSELoss
 * @brief 均方误差损失: loss = mean((input - target)^2) 或 sum((input - target)^2)
 *
 * 与 PyTorch nn.MSELoss API 一致。
 * 注意：MSELoss 不继承自 Module，因为 forward 接受 (input, target) 两个参数。
 */
template <typename T, typename Alloc = mem::Allocator<T>>
class MSELoss : public Object<MSELoss<T, Alloc>> {
public:
    using TensorT = Tensor<T, Alloc>;

    /**
     * @brief 构造 MSELoss
     * @param reduction 归约方式，"mean" 或 "sum"
     */
    explicit MSELoss(const CString& reduction = "mean")
        : reduction_(reduction) {}

    /**
     * @brief 计算损失
     * @param input 预测值
     * @param target 目标值
     * @return 损失值（标量张量）
     */
    TensorT forward(const TensorT& input, const TensorT& target) {
        TensorT diff = input.broadcast_sub(target);
        TensorT squared = diff.broadcast_mul(diff);
        TensorT loss;

        if (reduction_ == "sum") {
            loss = autograd_sum(squared);
        } else {
            // "mean"
            loss = autograd_sum(squared).broadcast_div(
                TensorT::scalar(static_cast<T>(squared.numel()))
            );
        }

        if (input.requires_grad() || target.requires_grad()) {
            loss._set_grad_fn(std::make_shared<MSELossBackward<T, Alloc>>(input, target, reduction_));
        }
        return loss;
    }

    /** @brief 归约方式 */
    CString reduction_;
};

// ==================== CrossEntropyLoss ====================

/**
 * @brief CrossEntropyLoss 反向传播
 * dL/dx_i = softmax(x_i) - y_i  （对交叉熵损失 + softmax 组合的梯度）
 */
template <typename T, typename Alloc = mem::Allocator<T>>
class CrossEntropyBackward : public GradFn<T, Alloc> {
public:
    using TensorT = Tensor<T, Alloc>;

    CrossEntropyBackward(const TensorT& input, const TensorT& target, const TensorT& softmax_out)
        : input_ptr_(&input), target_ptr_(&target), softmax_out_ptr_(&softmax_out) {}

    void backward(const TensorT& grad_output) override {
        // grad = softmax_out - one_hot(target) / batch_size
        // (交叉熵+softmax的联合梯度)
        const auto& input_ = *input_ptr_;
        const auto& target_ = *target_ptr_;
        const auto& softmax_out_ = *softmax_out_ptr_;
        usize batch = input_.shape()[0];
        usize num_classes = input_.shape()[1];

        TensorT grad(input_.shape());
        auto* gd = grad.data();
        auto* sm = softmax_out_.contiguous().data();
        auto* tg = target_.contiguous().data();

        for (usize b = 0; b < batch; ++b) {
            isize label = static_cast<isize>(tg[b]);
            for (usize c = 0; c < num_classes; ++c) {
                T one_hot = (static_cast<isize>(c) == label) ? static_cast<T>(1) : static_cast<T>(0);
                gd[b * num_classes + c] = (sm[b * num_classes + c] - one_hot) / static_cast<T>(batch);
            }
        }

        input_ptr_->_add_grad(grad.broadcast_mul(grad_output));
    }

    [[nodiscard]] CString to_string() const {
        return CString{"CrossEntropyBackward"};
    }

private:
    const TensorT* input_ptr_;
    const TensorT* target_ptr_;
    const TensorT* softmax_out_ptr_;
};

/**
 * @class CrossEntropyLoss
 * @brief 交叉熵损失（Softmax + Negative Log Likelihood 的组合）
 *
 * 与 PyTorch nn.CrossEntropyLoss API 一致。
 * input 形状: (batch, num_classes)
 * target 形状: (batch,)  每个元素为类别索引 [0, num_classes)
 * 注意：CrossEntropyLoss 不继承自 Module，因为 forward 接受两个参数。
 */
template <typename T, typename Alloc = mem::Allocator<T>>
class CrossEntropyLoss : public Object<CrossEntropyLoss<T, Alloc>> {
public:
    using TensorT = Tensor<T, Alloc>;

    /**
     * @brief 构造交叉熵损失
     */
    CrossEntropyLoss() = default;

    /**
     * @brief 计算损失
     * @param input 原始 logits，形状 (batch, num_classes)
     * @param target 目标类别索引，形状 (batch,)
     * @return 损失值（标量张量）
     */
    TensorT forward(const TensorT& input, const TensorT& target) {
        auto inp = input.contiguous();
        auto tgt = target.contiguous();

        usize batch = inp.shape()[0];
        usize num_classes = inp.shape()[1];

        // 数值稳定的 log_softmax: log_softmax = x - max - log(sum(exp(x - max)))
        T max_val = inp.max();
        T sum_exp = static_cast<T>(0);
        for (usize i = 0; i < inp.numel(); ++i) {
            sum_exp += std::exp(inp.data()[i] - max_val);
        }
        T log_sum_exp = max_val + std::log(sum_exp);

        // NLL: -log(softmax(x)[target])
        T loss_val = static_cast<T>(0);
        for (usize b = 0; b < batch; ++b) {
            isize label = static_cast<isize>(tgt.data()[b]);
            loss_val += -(inp.data()[b * num_classes + label] - log_sum_exp);
        }
        loss_val /= static_cast<T>(batch);

        TensorT result = TensorT::scalar(loss_val);

        // 计算 softmax 输出（用于反向传播）
        TensorT softmax_out(inp.shape());
        for (usize i = 0; i < inp.numel(); ++i) {
            softmax_out.data()[i] = std::exp(inp.data()[i] - log_sum_exp);
        }

        if (inp.requires_grad()) {
            result._set_grad_fn(
                std::make_shared<CrossEntropyBackward<T, Alloc>>(inp, tgt, softmax_out)
            );
        }
        return result;
    }
};

} // namespace my::nn

#endif // LOSS_HPP
