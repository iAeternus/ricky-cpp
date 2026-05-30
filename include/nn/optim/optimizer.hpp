/**
 * @brief 优化器基类
 * @author Ricky
 * @date 2026/5/30
 * @version 1.0
 */
#ifndef OPTIMIZER_HPP
#define OPTIMIZER_HPP

#include "../module.hpp"

namespace my::nn::optim {

/**
 * @class Optimizer
 * @brief 所有优化器的基类
 * @tparam T 张量元素类型
 * @tparam Alloc 内存分配器
 *
 * 管理参数列表，提供 step（更新参数）和 zero_grad（清零梯度）接口。
 */
template <typename T, typename Alloc = mem::Allocator<T>>
class Optimizer : public Object<Optimizer<T, Alloc>> {
public:
    using TensorT = Tensor<T, Alloc>;

    /**
     * @brief 构造优化器
     * @param params 待优化的参数指针列表
     */
    explicit Optimizer(util::Vec<TensorT*> params)
        : params_(std::move(params)) {}

    virtual ~Optimizer() = default;

    /**
     * @brief 更新参数
     *
     * 根据当前梯度计算参数更新量并应用到参数上。
     */
    virtual void step() = 0;

    /**
     * @brief 清零所有参数的梯度
     */
    virtual void zero_grad() {
        for (usize i = 0; i < params_.len(); ++i) {
            params_[i]->zero_grad();
        }
    }

protected:
    /** @brief 待优化的参数 */
    util::Vec<TensorT*> params_;
};

} // namespace my::nn::optim

#endif // OPTIMIZER_HPP
