/**
 * @brief 类 rust 迭代器适配器
 * @author Ricky
 * @date 2025/12/17
 * @version 1.0
 */
#ifndef ITER_HPP
#define ITER_HPP

#include "my_concepts.hpp"

namespace my::util {

/**
 * @brief 迭代器
 */
template <typename I>
concept Iterator = requires(I iter) {
    typename I::Item;

    /**
     * @brief 返回 &Item 表示有效元素，返回 nullptr 表示结束
     */
    { iter.next() } -> std::same_as<typename I::Item*>;

    // 可复制构造
    requires std::copy_constructible<I>;
    // 可移动构造
    requires std::move_constructible<I>;
};

template <typename T>
concept IntoIterator = requires(T value) {
    typename T::Iter;
    requires Iterator<typename T::Iter>;
    { value.into_iter() } -> std::same_as<typename T::Iter>;
};

/**
 * @brief 容器类型
 */
template <typename C>
concept Container = Iterable<C> && requires(C c) {
    typename C::value_t;
    { c.size() } -> std::convertible_to<usize>;
};

/**
 * @brief 迭代器结束标记
 */
struct EndSentinel {};

/**
 * @brief 迭代器CRTP基类
 */
template <typename D>
class IteratorBase {
public:
    using Item = typename D::Item;

    Item* next() {
        return static_cast<D*>(this)->next_impl();
    }

    template <typename F>
    auto map(F&& f) && {
        return MapIterator<D, std::decay_t<F>>(
            std::move(*static_cast<D*>(this)),
            std::forward<F>(f));
    }

    template <typename Pred>
    auto filter(Pred&& p) && {
        return FilterIterator<D, std::decay_t<Pred>>(
            std::move(*static_cast<D*>(this)),
            std::forward<Pred>(p));
    }

    template <typename F>
    void for_each(F&& f) {
        while (auto* item = next()) {
            f(*item);
        }
    }
};

template <typename Iter, typename Func>
class MapIterator : public IteratorBase<MapIterator<Iter, Func>> {
public:
    using SrcItem = typename Iter::Item;
    using Item = std::invoke_result_t<Func&, SrcItem&>;

    MapIterator(Iter iter, Func func) :
            iter_(std::move(iter)), func_(std::move(func)) {}

    Item* next_impl() {
        if (auto* src = iter_.next()) {
            value_ = func_(*src);
            return &value_;
        }
        return nullptr;
    }

private:
    Iter iter_;
    Func func_;
    Item value_; // Rust 的 yield 临时值
};

template <typename Iter, typename Pred>
class FilterIterator : public IteratorBase<FilterIterator<Iter, Pred>> {
public:
    using Item = typename Iter::Item;

    FilterIterator(Iter iter, Pred pred) :
            iter_(std::move(iter)), pred_(std::move(pred)) {}

    Item* next_impl() {
        while (auto* item = iter_.next()) {
            if (pred_(*item)) {
                return item;
            }
        }
        return nullptr;
    }

private:
    Iter iter_;
    Pred pred_;
};

} // namespace my::util

#endif // ITER_HPP
