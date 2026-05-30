/**
 * @brief SGD 优化器
 * @author Ricky
 * @date 2026/5/30
 * @version 1.0
 */
#ifndef SGD_HPP
#define SGD_HPP

#include "optimizer.hpp"

namespace my::nn::optim {

/**
 * @class SGD
 * @brief 随机梯度下降优化器
 * @tparam T 张量元素类型
 *
 * 支持动量（Momentum）和权重衰减（Weight Decay）。
 * 与 PyTorch torch.optim.SGD API 一致。
 *
 * 更新公式（无动量）：
 *   w = w - lr * (grad + weight_decay * w)
 *
 * 更新公式（有动量）：
 *   v = momentum * v + grad + weight_decay * w
 *   w = w - lr * v
 */
template <typename T, typename Alloc = mem::Allocator<T>>
class SGD : public Optimizer<T, Alloc> {
public:
    using TensorT = Tensor<T, Alloc>;
    using Base = Optimizer<T, Alloc>;

    /**
     * @brief 构造 SGD 优化器
     * @param params 待优化的参数
     * @param lr 学习率
     * @param momentum 动量因子
     * @param weight_decay 权重衰减系数
     * @param nesterov 是否使用 Nesterov 动量
     */
    SGD(util::Vec<TensorT*> params, T lr,
        T momentum = static_cast<T>(0),
        T weight_decay = static_cast<T>(0),
        bool nesterov = false)
        : Base(std::move(params))
        , lr_(lr), momentum_(momentum)
        , weight_decay_(weight_decay), nesterov_(nesterov) {

        if (momentum_ != static_cast<T>(0)) {
            for (usize i = 0; i < Base::params_.len(); ++i) {
                auto* p = Base::params_[i];
                momentum_bufs_.push(TensorT::zeros(p->shape()));
            }
        }
    }

    /**
     * @brief 执行一步参数更新
     */
    void step() override {
        for (usize i = 0; i < Base::params_.len(); ++i) {
            auto* p = Base::params_[i];
            auto& g = p->grad();

            if (g.is_empty()) continue;

            TensorT grad = g;

            // 权重衰减
            if (weight_decay_ != static_cast<T>(0)) {
                grad = grad.broadcast_add(
                    p->broadcast_mul(TensorT::scalar(weight_decay_)));
            }

            if (momentum_ != static_cast<T>(0)) {
                auto& v = momentum_bufs_[i];
                // v = momentum * v + grad
                v = v.broadcast_mul(TensorT::scalar(momentum_))
                      .broadcast_add(grad);

                if (nesterov_) {
                    // w = w - lr * (grad + momentum * v)
                    TensorT update = grad.broadcast_add(
                        v.broadcast_mul(TensorT::scalar(momentum_)));
                    *p = p->broadcast_sub(
                        update.broadcast_mul(TensorT::scalar(lr_)));
                    p->set_requires_grad(true);
                } else {
                    // w = w - lr * v
                    *p = p->broadcast_sub(
                        v.broadcast_mul(TensorT::scalar(lr_)));
                    p->set_requires_grad(true);
                }
            } else {
                // w = w - lr * grad
                *p = p->broadcast_sub(
                    grad.broadcast_mul(TensorT::scalar(lr_)));
                p->set_requires_grad(true);
            }
        }
    }

private:
    T lr_;
    T momentum_;
    T weight_decay_;
    bool nesterov_;
    util::Vec<TensorT> momentum_bufs_;
};

} // namespace my::nn::optim

#endif // SGD_HPP
