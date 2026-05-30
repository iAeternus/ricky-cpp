#ifndef NN_DATASET_HPP
#define NN_DATASET_HPP

#include "nn/tensor.hpp"
#include "util/random.hpp"
#include "fs/file.hpp"
#include "my_pair.hpp"

#include <sstream>

namespace my::nn::data {

template <typename T, typename Alloc = mem::Allocator<T>>
class Dataset {
public:
    using TensorT = Tensor<T, Alloc>;

    virtual ~Dataset() = default;
    virtual usize size() const = 0;
    virtual Pair<TensorT, TensorT> get(usize idx) const = 0;
};

template <typename T, typename Alloc = mem::Allocator<T>>
class TensorDataset : public Dataset<T, Alloc> {
public:
    using TensorT = Tensor<T, Alloc>;

    TensorDataset() = default;

    explicit TensorDataset(const util::Vec<Pair<TensorT, TensorT>>& samples)
        : samples_(samples) {}

    TensorDataset(const TensorT& x, const TensorT& y) {
        usize n = x.shape()[0];
        usize num_features = x.shape()[1];
        typename TensorT::Shape xshape;
        xshape.push(num_features);
        for (usize i = 0; i < n; ++i) {
            TensorT xi(xshape);
            for (usize j = 0; j < num_features; ++j) {
                xi.data()[j] = const_cast<TensorT&>(x)(i, j);
            }
            T yval = y.ndim() == 1 ? const_cast<TensorT&>(y)(i) : const_cast<TensorT&>(y)(i, 0);
            TensorT yi = TensorT::scalar(yval);
            samples_.push(Pair<TensorT, TensorT>(std::move(xi), std::move(yi)));
        }
    }

    TensorDataset(const TensorDataset& other, const util::Vec<usize>& indices) {
        for (usize i = 0; i < indices.len(); ++i) {
            samples_.push(other.samples_[indices[i]]);
        }
    }

    void push(const TensorT& x, const TensorT& y) {
        samples_.push(Pair<TensorT, TensorT>(x, y));
    }

    usize size() const override {
        return samples_.len();
    }

    Pair<TensorT, TensorT> get(usize idx) const override {
        return samples_[idx];
    }

    const TensorT& get_input(usize idx) const {
        return samples_[idx].first();
    }

    const TensorT& get_target(usize idx) const {
        return samples_[idx].second();
    }

private:
    util::Vec<Pair<TensorT, TensorT>> samples_;
};

template <typename T, typename Alloc = mem::Allocator<T>>
class CSVDataset {
public:
    using TensorT = Tensor<T, Alloc>;
    using Shape = typename TensorT::Shape;

    CSVDataset(const CString& csv_path, usize num_features,
               f32 val_ratio = 0.15, f32 test_ratio = 0.15)
        : num_features_(num_features) {
        auto file = fs::File::open(csv_path);
        auto content = file.read_all();
        auto data_cstr = content.as_cstr();

        util::Vec<TensorT> inputs;
        util::Vec<TensorT> targets;

        std::stringstream ss(static_cast<const char*>(data_cstr));
        std::string line;
        while (std::getline(ss, line)) {
            if (line.empty()) continue;

            TensorT xi(Shape{num_features});
            std::stringstream ls(line);
            std::string token;

            for (usize j = 0; j < num_features; ++j) {
                std::getline(ls, token, ',');
                xi.data()[j] = static_cast<T>(std::stof(token));
            }

            std::getline(ls, token, ',');
            TensorT yi(Shape{1});
            yi.data()[0] = static_cast<T>(std::stof(token));

            inputs.push(std::move(xi));
            targets.push(std::move(yi));
        }

        usize n = inputs.len();

        util::Vec<usize> indices;
        for (usize i = 0; i < n; ++i) {
            indices.push(i);
        }

        util::Random::thread_local_rng().shuffle(indices.begin(), indices.end());

        usize val_count = static_cast<usize>(n * val_ratio);
        usize test_count = static_cast<usize>(n * test_ratio);
        usize train_count = n - val_count - test_count;

        for (usize i = 0; i < train_count; ++i) {
            train_data_.push(inputs[indices[i]], targets[indices[i]]);
        }
        for (usize i = train_count; i < train_count + val_count; ++i) {
            val_data_.push(inputs[indices[i]], targets[indices[i]]);
        }
        for (usize i = train_count + val_count; i < n; ++i) {
            test_data_.push(inputs[indices[i]], targets[indices[i]]);
        }
    }

    const TensorDataset<T, Alloc>& train_set() const { return train_data_; }
    const TensorDataset<T, Alloc>& val_set() const { return val_data_; }
    const TensorDataset<T, Alloc>& test_set() const { return test_data_; }

    usize num_features() const { return num_features_; }

private:
    usize num_features_;
    TensorDataset<T, Alloc> train_data_;
    TensorDataset<T, Alloc> val_data_;
    TensorDataset<T, Alloc> test_data_;
};

} // namespace my::nn::data

#endif // NN_DATASET_HPP
