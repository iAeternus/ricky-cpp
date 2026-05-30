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
    using Shape = typename TensorT::Shape;

    explicit MSELoss(const CString& reduction = "mean", T weight_decay = static_cast<T>(0))
        : reduction_(reduction), weight_decay_(weight_decay) {}

    void set_params(const util::Vec<TensorT*>& params) {
        params_ = params;
    }

    TensorT forward(const TensorT& input, const TensorT& target) {
        TensorT diff = input.broadcast_sub(target);
        TensorT squared = diff.broadcast_mul(diff);
        TensorT loss;

        if (reduction_ == "sum") {
            loss = autograd_sum(squared);
        } else {
            loss = autograd_sum(squared).broadcast_div(
                TensorT::scalar(static_cast<T>(squared.numel()))
            );
        }

        if (weight_decay_ > static_cast<T>(0) && params_.len() > 0) {
            T reg = static_cast<T>(0);
            for (usize i = 0; i < params_.len(); ++i) {
                auto& p = *params_[i];
                for (usize j = 0; j < p.numel(); ++j) {
                    T val = p.data()[j];
                    reg += val * val;
                }
            }
            reg *= weight_decay_ / static_cast<T>(2);
            loss = TensorT::scalar(loss.data()[0] + reg);
        }

        if (input.requires_grad() || target.requires_grad()) {
            loss._set_grad_fn(std::make_shared<MSELossBackward<T, Alloc>>(input, target, reduction_));
        }
        return loss;
    }

    CString reduction_;
    T weight_decay_ = static_cast<T>(0);
    util::Vec<TensorT*> params_;
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
    using Shape = typename TensorT::Shape;

    explicit CrossEntropyLoss(T weight_decay = static_cast<T>(0))
        : weight_decay_(weight_decay) {}

    void set_params(const util::Vec<TensorT*>& params) {
        params_ = params;
    }

    TensorT forward(const TensorT& input, const TensorT& target) {
        cached_inp_ = input.contiguous();
        cached_tgt_ = target.contiguous();

        usize batch = cached_inp_.shape()[0];
        usize num_classes = cached_inp_.shape()[1];

        // 逐样本计算 log-sum-exp（数值稳定的 softmax）
        cached_softmax_out_ = TensorT(cached_inp_.shape());
        T loss_val = static_cast<T>(0);

        for (usize b = 0; b < batch; ++b) {
            usize base = b * num_classes;

            // 找当前样本的最大 logit
            T max_val = cached_inp_.data()[base];
            for (usize c = 1; c < num_classes; ++c) {
                if (cached_inp_.data()[base + c] > max_val) {
                    max_val = cached_inp_.data()[base + c];
                }
            }

            // 逐样本 softmax + log-sum-exp
            T sum_exp = static_cast<T>(0);
            for (usize c = 0; c < num_classes; ++c) {
                T e = std::exp(cached_inp_.data()[base + c] - max_val);
                cached_softmax_out_.data()[base + c] = e;
                sum_exp += e;
            }

            // 归一化 softmax
            for (usize c = 0; c < num_classes; ++c) {
                cached_softmax_out_.data()[base + c] /= sum_exp;
            }

            T log_sum_exp = max_val + std::log(sum_exp);
            isize label = static_cast<isize>(cached_tgt_.data()[b]);
            loss_val += -(cached_inp_.data()[base + label] - log_sum_exp);
        }

        loss_val /= static_cast<T>(batch);

        if (weight_decay_ > static_cast<T>(0) && params_.len() > 0) {
            T reg = static_cast<T>(0);
            for (usize i = 0; i < params_.len(); ++i) {
                auto& p = *params_[i];
                for (usize j = 0; j < p.numel(); ++j) {
                    T val = p.data()[j];
                    reg += val * val;
                }
            }
            reg *= weight_decay_ / static_cast<T>(2);
            loss_val += reg;
        }

        TensorT result = TensorT::scalar(loss_val);

        if (cached_inp_.requires_grad()) {
            result._set_grad_fn(
                std::make_shared<CrossEntropyBackward<T, Alloc>>(cached_inp_, cached_tgt_, cached_softmax_out_)
            );
        }
        return result;
    }

    T weight_decay_ = static_cast<T>(0);
    util::Vec<TensorT*> params_;

private:
    TensorT cached_inp_;
    TensorT cached_tgt_;
    TensorT cached_softmax_out_;
};

} // namespace my::nn

#endif // LOSS_HPP
