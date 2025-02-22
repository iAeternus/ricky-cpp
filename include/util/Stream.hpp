/**
 * @brief 流式处理框架，支持协程生成器、惰性求值和并行处理
 * @author Ricky
 * @date 2025/2/2
 * @version 1.0
 */
#ifndef STREAM_HPP
#define STREAM_HPP

#include "Generator.hpp"
#include "DynArray.hpp"

#include <iterator>
#include <execution>
#include <mutex>

namespace my::util {

template <typename Derived, typename T>
class Stream;

template <typename T>
class IterStream;

template <typename Upstream, typename Pred, typename T>
class FilterStream;

template <typename Upstream, typename Mapper, typename T>
class MapStream;

/**
 * @brief 流CRTP基类
 */
template <typename Derived, typename T>
class Stream {
public:
    using value_t = T;

    /**
     * @brief 中间操作：过滤
     */
    template <typename Pred>
    auto filter(Pred&& pred) && {
        return FilterStream<Derived, Pred, value_t>(
            std::move(static_cast<Derived&>(*this)),
            std::forward<Pred>(pred));
    }

    /**
     * @brief 中间操作：映射
     */
    template <typename Mapper>
    auto map(Mapper&& func) && {
        using RetType = std::invoke_result_t<Mapper, value_t>;
        return MapStream<Derived, Mapper, RetType>(
            std::move(static_cast<Derived&>(*this)),
            std::forward<Mapper>(func));
    }

    /**
     * @brief 终止操作：收集到容器 TODO 暂定为DynArray
     */
    auto collect() && -> util::DynArray<value_t> {
        util::DynArray<value_t> result;
        for (auto&& elem : static_cast<Derived&>(*this).generator()) {
            result.append(std::forward<decltype(elem)>(elem));
        }
        return result;
    }

    /**
     * @brief 终止操作：遍历元素
     */
    template <typename Action>
    void forEach(Action&& action) && {
        for (auto&& elem : static_cast<Derived&>(*this).generator()) {
            action(std::forward<decltype(elem)>(elem));
        }
    }
};

/**
 * @brief 迭代器流
 */
template <typename T>
class IterStream : public Stream<IterStream<T>, typename std::iterator_traits<T>::value_type> {
public:
    using value_t = typename std::iterator_traits<T>::value_type;

    IterStream(T begin, T end) :
            begin_(begin), end_(end) {}

    coro::Generator<value_t> generator() const {
        for (auto it = begin_; it != end_; ++it) {
            co_yield* it;
        }
    }

private:
    T begin_;
    T end_;
};

/**
 * @brief 过滤流
 */
template <typename Upstream, typename Pred, typename T>
class FilterStream : public Stream<FilterStream<Upstream, Pred, T>, T> {
public:
    FilterStream(Upstream&& upstream, Pred&& pred) :
            upstream_(std::move(upstream)), pred_(std::move(pred)) {}

    coro::Generator<T> generator() const {
        for (auto&& elem : upstream_.generator()) {
            if (pred_(elem)) {
                co_yield std::forward<decltype(elem)>(elem);
            }
        }
    }

private:
    Upstream upstream_;
    Pred pred_;
};

/**
 * @brief 映射流
 */
template <typename Upstream, typename Mapper, typename RetType>
class MapStream : public Stream<MapStream<Upstream, Mapper, RetType>, RetType> {
public:
    MapStream(Upstream&& upstream, Mapper&& func) :
            upstream_(std::move(upstream)), func_(std::move(func)) {}

    coro::Generator<RetType> generator() const {
        for (auto&& elem : upstream_.generator()) {
            co_yield func_(std::forward<decltype(elem)>(elem));
        }
    }

private:
    Upstream upstream_;
    Mapper func_;
};

// Stream工厂
template <Iterable I>
fn stream(I&& iter) {
    return IterStream(iter.begin(), iter.end());
}

} // namespace my::util

#endif // STREAM_HPP