/**
 * @brief 函数式 API（仿 PyTorch F.*）
 * @author Ricky
 * @date 2026/5/30
 * @version 1.0
 *
 * 提供无状态函数形式的神经网络操作。
 * 所有函数均支持自动微分。
 */
#ifndef FUNCTIONAL_HPP
#define FUNCTIONAL_HPP

#include "autograd.hpp"
#include "activations.hpp"
#include "loss.hpp"
#include "linear.hpp"

namespace my::nn::F {

// ==================== 激活函数 ====================

/**
 * @brief ReLU 函数: max(0, x)
 */
template <typename T, typename Alloc = mem::Allocator<T>>
Tensor<T, Alloc> relu(const Tensor<T, Alloc>& input) {
    ReLU<T, Alloc> act;
    return act.forward(input);
}

/**
 * @brief Sigmoid 函数: 1 / (1 + exp(-x))
 */
template <typename T, typename Alloc = mem::Allocator<T>>
Tensor<T, Alloc> sigmoid(const Tensor<T, Alloc>& input) {
    Sigmoid<T, Alloc> act;
    return act.forward(input);
}

/**
 * @brief Softmax 函数
 * @param input 输入张量
 * @param dim 计算维度
 */
template <typename T, typename Alloc = mem::Allocator<T>>
Tensor<T, Alloc> softmax(const Tensor<T, Alloc>& input, isize dim = -1) {
    Softmax<T, Alloc> act(dim);
    return act.forward(input);
}

// ==================== 损失函数 ====================

/**
 * @brief MSE 损失
 * @param input 预测值
 * @param target 目标值
 * @param reduction 归约方式，"mean" 或 "sum"
 */
template <typename T, typename Alloc = mem::Allocator<T>>
Tensor<T, Alloc> mse_loss(const Tensor<T, Alloc>& input, const Tensor<T, Alloc>& target,
                           const CString& reduction = "mean") {
    MSELoss<T, Alloc> loss(reduction);
    return loss.forward(input, target);
}

/**
 * @brief 交叉熵损失
 * @param input 原始 logits，形状 (batch, num_classes)
 * @param target 目标类别索引，形状 (batch,)
 */
template <typename T, typename Alloc = mem::Allocator<T>>
Tensor<T, Alloc> cross_entropy(const Tensor<T, Alloc>& input, const Tensor<T, Alloc>& target) {
    CrossEntropyLoss<T, Alloc> loss;
    return loss.forward(input, target);
}

// ==================== 线性层 ====================

/**
 * @brief 线性变换: y = x @ W^T + b
 * @param input 输入张量
 * @param weight 权重张量
 * @param bias 偏置张量（可选）
 */
template <typename T, typename Alloc = mem::Allocator<T>>
Tensor<T, Alloc> linear(const Tensor<T, Alloc>& input, const Tensor<T, Alloc>& weight,
                         const Tensor<T, Alloc>& bias = Tensor<T, Alloc>()) {
    Tensor<T, Alloc> w_t = weight.transpose(0, 1);
    Tensor<T, Alloc> out = autograd_matmul(input, w_t);
    if (!bias.is_empty()) {
        out = autograd_add(out, bias);
    }
    return out;
}

} // namespace my::nn::F

#endif // FUNCTIONAL_HPP
