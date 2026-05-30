/**
 * @brief 线性层（全连接层）
 * @author Ricky
 * @date 2026/5/30
 * @version 1.0
 */
#ifndef LINEAR_HPP
#define LINEAR_HPP

#include "module.hpp"
#include "autograd.hpp"

namespace my::nn {

/**
 * @class Linear
 * @brief 线性变换层 y = x @ W^T + b
 * @tparam T 张量元素类型
 *
 * 与 PyTorch nn.Linear API 一致。
 * weight 形状为 (out_features, in_features)
 * bias 形状为 (out_features,)
 */
template <typename T, typename Alloc = mem::Allocator<T>>
class Linear : public Module<T, Alloc> {
public:
    using TensorT = Tensor<T, Alloc>;
    using Shape = typename TensorT::Shape;

    /**
     * @brief 构造线性层
     * @param in_features 输入特征数
     * @param out_features 输出特征数
     * @param use_bias 是否使用偏置
     *
     * weight 使用均匀分布 U(-sqrt(1/in_features), sqrt(1/in_features)) 初始化
     * bias 初始化为 0
     */
    Linear(usize in_features, usize out_features, bool use_bias = true)
        : in_features_(in_features), out_features_(out_features) {

        // weight: (out_features, in_features)
        T bound = static_cast<T>(1) / std::sqrt(static_cast<T>(in_features));
        weight_ = TensorT::rand(Shape{out_features, in_features});
        for (usize i = 0; i < weight_.numel(); ++i) {
            weight_.data()[i] = weight_.data()[i] * static_cast<T>(2) * bound - bound;
        }
        this->register_param(weight_);

        // bias: (out_features,)
        if (use_bias) {
            bias_ = TensorT::zeros(Shape{out_features});
            this->register_param(bias_);
        }
    }

    /**
     * @brief 前向传播
     * @param input 输入张量，形状 (*, in_features)
     * @return 输出张量，形状 (*, out_features)
     *
     * 权重转置和中间结果缓存为成员变量，确保 GradFn 中存储的
     * 原始指针在 backward() 完成前始终有效。
     * backward() 必须在下次 forward() 前调用。
     */
    TensorT forward(const TensorT& input) override {
        cached_input_ = input;
        w_t_ = autograd_transpose(weight_, static_cast<usize>(0), static_cast<usize>(1));
        matmul_out_ = autograd_matmul(cached_input_, w_t_);
        if (!bias_.is_empty()) {
            return autograd_add(matmul_out_, bias_);
        }
        return matmul_out_;
    }

    util::Vec<TensorT*> parameters() override {
        util::Vec<TensorT*> params;
        params.push(&weight_);
        if (!bias_.is_empty()) {
            params.push(&bias_);
        }
        return params;
    }

    TensorT weight_;
    TensorT bias_;

private:
    usize in_features_;
    usize out_features_;
    TensorT cached_input_;
    TensorT w_t_;
    TensorT matmul_out_;
};

} // namespace my::nn

#endif // LINEAR_HPP
