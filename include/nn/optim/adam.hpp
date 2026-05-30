/**
 * @brief Adam 优化器
 * @author Ricky
 * @date 2026/5/30
 * @version 1.0
 */
#ifndef ADAM_HPP
#define ADAM_HPP

#include "optimizer.hpp"

namespace my::nn::optim {

/**
 * @class Adam
 * @brief Adam 优化器（Adaptive Moment Estimation）
 * @tparam T 张量元素类型
 *
 * 与 PyTorch torch.optim.Adam API 一致。
 *
 * 更新公式：
 *   m_t = beta1 * m_{t-1} + (1 - beta1) * g_t
 *   v_t = beta2 * v_{t-1} + (1 - beta2) * g_t^2
 *   m_hat = m_t / (1 - beta1^t)
 *   v_hat = v_t / (1 - beta2^t)
 *   w_t = w_{t-1} - lr * m_hat / (sqrt(v_hat) + eps)
 */
template <typename T, typename Alloc = mem::Allocator<T>>
class Adam : public Optimizer<T, Alloc> {
public:
    using TensorT = Tensor<T, Alloc>;
    using Base = Optimizer<T, Alloc>;

    /**
     * @brief 构造 Adam 优化器
     * @param params 待优化的参数
     * @param lr 学习率
     * @param beta1 一阶矩衰减系数
     * @param beta2 二阶矩衰减系数
     * @param eps 数值稳定性常数
     * @param weight_decay 权重衰减系数
     */
    Adam(util::Vec<TensorT*> params,
         T lr = static_cast<T>(1e-3),
         T beta1 = static_cast<T>(0.9),
         T beta2 = static_cast<T>(0.999),
         T eps = static_cast<T>(1e-8),
         T weight_decay = static_cast<T>(0)) :
            Base(std::move(params)), lr_(lr), beta1_(beta1), beta2_(beta2), eps_(eps), weight_decay_(weight_decay), step_count_(0) {
        for (usize i = 0; i < Base::params_.len(); ++i) {
            auto* p = Base::params_[i];
            m_bufs_.push(TensorT::zeros(p->shape()));
            v_bufs_.push(TensorT::zeros(p->shape()));
        }
    }

    /**
     * @brief 执行一步参数更新
     */
    void step() override {
        step_count_++;

        T bias_correction1 = static_cast<T>(1) - std::pow(beta1_, static_cast<int>(step_count_));
        T bias_correction2 = static_cast<T>(1) - std::pow(beta2_, static_cast<int>(step_count_));

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

            // 更新一阶矩: m = beta1 * m + (1 - beta1) * grad
            m_bufs_[i] = m_bufs_[i].broadcast_mul(TensorT::scalar(beta1_)).broadcast_add(grad.broadcast_mul(TensorT::scalar(static_cast<T>(1) - beta1_)));

            // 更新二阶矩: v = beta2 * v + (1 - beta2) * grad^2
            v_bufs_[i] = v_bufs_[i].broadcast_mul(TensorT::scalar(beta2_)).broadcast_add(grad.broadcast_mul(grad).broadcast_mul(TensorT::scalar(static_cast<T>(1) - beta2_)));

            // 偏差矫正
            TensorT m_hat = m_bufs_[i].broadcast_div(TensorT::scalar(bias_correction1));
            TensorT v_hat = v_bufs_[i].broadcast_div(TensorT::scalar(bias_correction2));

            // 计算更新步: -lr * m_hat / (sqrt(v_hat) + eps)
            TensorT denom = v_hat.broadcast_pow(static_cast<T>(0.5))
                                .broadcast_add(TensorT::scalar(eps_));
            TensorT update = m_hat.broadcast_div(denom)
                                 .broadcast_mul(TensorT::scalar(lr_));

            *p = p->broadcast_sub(update);
            p->set_requires_grad(true);
        }
    }

private:
    T lr_;
    T beta1_;
    T beta2_;
    T eps_;
    T weight_decay_;
    usize step_count_;
    util::Vec<TensorT> m_bufs_; // 一阶矩
    util::Vec<TensorT> v_bufs_; // 二阶矩
};

} // namespace my::nn::optim

#endif // ADAM_HPP
