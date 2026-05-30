/**
 * @brief 模块基类
 * @author Ricky
 * @date 2026/5/30
 * @version 1.0
 */
#ifndef MODULE_HPP
#define MODULE_HPP

#include "tensor.hpp"
#include "fs/file.hpp"

namespace my::nn {

/**
 * @class Module
 * @brief 所有神经网络模块的基类
 * @tparam T 张量元素类型（通常为 f32 或 f64）
 *
 * 提供参数管理、训练/评估模式切换、梯度清零等通用功能。
 * 子类需实现 forward() 方法。
 *
 * 用法：
 * @code
 * class MyLayer : public Module<f32> {
 *     Tensor<f32> weight_;
 * public:
 *     MyLayer() { register_param(weight_); }
 *     Tensor<f32> forward(const Tensor<f32>& input) override { ... }
 * };
 * @endcode
 */
template <typename T, typename Alloc = mem::Allocator<T>>
class Module : public Object<Module<T, Alloc>> {
public:
    using TensorT = Tensor<T, Alloc>;
    using Self = Module<T, Alloc>;

    virtual ~Module() = default;

    /**
     * @brief 前向传播
     * @param input 输入张量
     * @return 输出张量
     */
    virtual TensorT forward(const TensorT& input) = 0;

    /**
     * @brief 函数调用运算符，委托给 forward
     */
    TensorT operator()(const TensorT& input) {
        return forward(input);
    }

    /**
     * @brief 获取所有可训练参数
     * @return 参数指针列表
     *
     * 递归收集当前模块及所有子模块的 parameters()。
     */
    virtual util::Vec<TensorT*> parameters() {
        util::Vec<TensorT*> result;
        _collect_params(result);
        return result;
    }

    /**
     * @brief 清零所有参数梯度
     */
    void zero_grad() {
        auto params = parameters();
        for (usize i = 0; i < params.len(); ++i) {
            params[i]->zero_grad();
        }
    }

    /**
     * @brief 设置为训练模式
     * @param mode 是否为训练模式
     */
    void train(bool mode = true) {
        training_ = mode;
    }

    /**
     * @brief 设置为评估模式
     */
    void eval() {
        train(false);
    }

    /**
     * @brief 判断是否为训练模式
     * @return 训练模式
     */
    [[nodiscard]] bool is_training() const noexcept {
        return training_;
    }

    /**
     * @brief 保存模型参数到二进制文件
     * @param path 文件路径
     *
     * 格式: magic(4B "MODL") + [ndim(8B) + shape(8B*ndim) + data(numel*sizeof(T))] × N
     */
    void save_params(const CString& path) const {
        auto params = const_cast<Self*>(this)->parameters();
        auto file = fs::File::create(path);

        i32 magic = 0x4D4F444C;
        file.write(reinterpret_cast<const char*>(&magic), sizeof(magic));

        for (usize i = 0; i < params.len(); ++i) {
            const auto& p = *params[i];
            auto shape = p.shape();
            usize ndim = shape.len();
            usize numel = p.numel();

            file.write(reinterpret_cast<const char*>(&ndim), sizeof(ndim));
            for (usize d = 0; d < ndim; ++d) {
                usize dim = shape[d];
                file.write(reinterpret_cast<const char*>(&dim), sizeof(dim));
            }
            file.write(reinterpret_cast<const char*>(p.data()), numel * sizeof(T));
        }
        file.flush();
    }

    /**
     * @brief 从二进制文件加载模型参数
     * @param path 文件路径
     *
     * 读取 save_params 写入的格式，按顺序填充各参数。
     * 调用前需确保模型结构（参数数量、形状）与保存时一致。
     */
    void load_params(const CString& path) {
        auto file = fs::File::open(path);
        auto content = file.read_all();
        const u8* bytes = content.as_bytes();
        usize remaining = content.len();

        auto read_usize = [&]() -> usize {
            usize val;
            std::memcpy(&val, bytes, sizeof(usize));
            bytes += sizeof(usize);
            remaining -= sizeof(usize);
            return val;
        };

        i32 magic;
        std::memcpy(&magic, bytes, sizeof(i32));
        bytes += sizeof(i32);
        remaining -= sizeof(i32);

        auto params = parameters();
        for (usize i = 0; i < params.len(); ++i) {
            auto& p = *params[i];
            usize ndim = read_usize();
            typename TensorT::Shape shape;
            for (usize d = 0; d < ndim; ++d) {
                shape.push(read_usize());
            }
            usize numel = p.numel();
            std::memcpy(p.data(), bytes, numel * sizeof(T));
            bytes += numel * sizeof(T);
            remaining -= numel * sizeof(T);
        }
    }

protected:
    /**
     * @brief 注册参数
     * @param tensor 可训练张量
     *
     * 将张量注册为模块的可训练参数，自动设置 requires_grad = true。
     */
    void register_param(TensorT& tensor) {
        tensor.set_requires_grad(true);
        registered_params_.push(&tensor);
    }

    /**
     * @brief 注册子模块
     * @param module 子模块
     *
     * 将子模块纳入参数管理，便于递归收集 parameters()。
     */
    void register_module(Module<T, Alloc>* module) {
        submodules_.push(module);
    }

    /**
     * @brief 训练模式标志
     */
    bool training_ = true;

private:
    void _collect_params(util::Vec<TensorT*>& result) {
        for (usize i = 0; i < registered_params_.len(); ++i) {
            result.push(registered_params_[i]);
        }
        for (usize i = 0; i < submodules_.len(); ++i) {
            submodules_[i]->_collect_params(result);
        }
    }

    util::Vec<TensorT*> registered_params_;
    util::Vec<Module<T, Alloc>*> submodules_;
};

} // namespace my::nn

#endif // MODULE_HPP
