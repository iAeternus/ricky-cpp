#ifndef NN_DATALOADER_HPP
#define NN_DATALOADER_HPP

#include "nn/data/dataset.hpp"
#include "util/random.hpp"

namespace my::nn::data {

template <typename T, typename Alloc = mem::Allocator<T>>
class DataLoader {
public:
    using TensorT = Tensor<T, Alloc>;
    using Shape = typename TensorT::Shape;

    DataLoader(const Dataset<T, Alloc>& dataset, usize batch_size, bool shuffle = true)
        : dataset_(&dataset), batch_size_(batch_size), shuffle_(shuffle) {
        reset();
    }

    void reset() {
        indices_.clear();
        for (usize i = 0; i < dataset_->size(); ++i) {
            indices_.push(i);
        }
        if (shuffle_) {
            util::Random::thread_local_rng().shuffle(indices_.begin(), indices_.end());
        }
        pos_ = 0;
    }

    bool has_next() const {
        return pos_ < indices_.len();
    }

    Pair<TensorT, TensorT> next_batch() {
        usize remaining = indices_.len() - pos_;
        usize bs = (remaining < batch_size_) ? remaining : batch_size_;
        usize num_features = 0;

        auto first = dataset_->get(indices_[pos_]);
        num_features = first.first().shape()[0];

        TensorT batch_x(Shape{bs, num_features});
        TensorT batch_y(Shape{bs, 1});

        for (usize i = 0; i < bs; ++i) {
            auto sample = dataset_->get(indices_[pos_ + i]);
            auto& x = sample.first();
            auto& y = sample.second();
            for (usize j = 0; j < num_features; ++j) {
                batch_x(i, j) = x.data()[j];
            }
            batch_y(i, 0) = y.data()[0];
        }

        pos_ += bs;
        return Pair<TensorT, TensorT>(std::move(batch_x), std::move(batch_y));
    }

    usize batch_size() const { return batch_size_; }
    usize size() const { return dataset_->size(); }

private:
    const Dataset<T, Alloc>* dataset_;
    usize batch_size_;
    bool shuffle_;
    util::Vec<usize> indices_;
    usize pos_ = 0;
};

} // namespace my::nn::data

#endif // NN_DATALOADER_HPP
