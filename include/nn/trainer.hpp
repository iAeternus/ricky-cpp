#ifndef NN_TRAINER_HPP
#define NN_TRAINER_HPP

#include "nn/module.hpp"
#include "nn/data/dataloader.hpp"
#include "nn/optim/optimizer.hpp"
#include "io/printer.hpp"

namespace my::nn {

template <typename T, typename Alloc = mem::Allocator<T>>
class Trainer {
public:
    using TensorT = Tensor<T, Alloc>;

    Trainer(Module<T, Alloc>& model,
            optim::Optimizer<T, Alloc>& optimizer,
            T l2_lambda = static_cast<T>(0))
        : model_(&model), optimizer_(&optimizer), l2_lambda_(l2_lambda) {}

    template <typename LossFn>
    T train_one_epoch(data::DataLoader<T, Alloc>& train_loader, LossFn& loss_fn) {
        T total_loss = static_cast<T>(0);
        usize batches = 0;

        train_loader.reset();
        while (train_loader.has_next()) {
            auto batch = train_loader.next_batch();
            auto& batch_x = batch.first();
            auto& batch_y = batch.second();

            optimizer_->zero_grad();
            auto pred = model_->forward(batch_x);
            auto loss = loss_fn.forward(pred, batch_y);

            if (l2_lambda_ > static_cast<T>(0)) {
                T reg = static_cast<T>(0);
                auto params = model_->parameters();
                for (usize i = 0; i < params.len(); ++i) {
                    auto& p = *params[i];
                    for (usize j = 0; j < p.numel(); ++j) {
                        T val = p.data()[j];
                        reg += val * val;
                    }
                }
                reg *= l2_lambda_ / static_cast<T>(2);
                auto loss_val = loss.data()[0] + reg;
                loss = TensorT::scalar(loss_val);
            }

            loss.backward();
            optimizer_->step();

            total_loss += loss.data()[0];
            ++batches;
        }

        return total_loss / static_cast<T>(batches);
    }

    template <typename LossFn>
    Pair<T, T> evaluate(data::DataLoader<T, Alloc>& eval_loader,
                        LossFn& loss_fn, bool is_classification) {
        T total_loss = static_cast<T>(0);
        usize correct = 0;
        usize total = 0;

        eval_loader.reset();
        while (eval_loader.has_next()) {
            auto batch = eval_loader.next_batch();
            auto& batch_x = batch.first();
            auto& batch_y = batch.second();

            auto pred = model_->forward(batch_x);
            auto loss = loss_fn.forward(pred, batch_y);
            total_loss += loss.data()[0];

            if (is_classification) {
                usize batch_size = batch_x.shape()[0];
                usize num_classes = pred.shape()[1];
                for (usize i = 0; i < batch_size; ++i) {
                    T max_val = pred(i, 0);
                    usize pred_class = 0;
                    for (usize j = 1; j < num_classes; ++j) {
                        if (pred(i, j) > max_val) {
                            max_val = pred(i, j);
                            pred_class = j;
                        }
                    }
                    isize true_class = static_cast<isize>(batch_y(i, 0));
                    if (static_cast<isize>(pred_class) == true_class) {
                        ++correct;
                    }
                }
                total += batch_size;
            }
        }

        T avg_loss = total_loss / static_cast<T>(eval_loader.size() / eval_loader.batch_size() + 1);
        T accuracy = is_classification
            ? static_cast<T>(correct) / static_cast<T>(total) * static_cast<T>(100)
            : static_cast<T>(0);

        return Pair<T, T>(avg_loss, accuracy);
    }

    template <typename LossFn>
    void train(data::DataLoader<T, Alloc>& train_loader,
               data::DataLoader<T, Alloc>& val_loader,
               LossFn& loss_fn, i32 epochs,
               bool is_classification = true) {
        for (i32 epoch = 0; epoch < epochs; ++epoch) {
            auto train_loss = train_one_epoch(train_loader, loss_fn);

            if (epoch % 50 == 0 || epoch == epochs - 1) {
                if (is_classification) {
                    auto eval_result = evaluate(val_loader, loss_fn, true);
                    io::println("Epoch ", epoch, " | Train loss: ", train_loss,
                                " | Val loss: ", eval_result.first(),
                                " | Val acc: ", eval_result.second(), "%");
                } else {
                    auto eval_result = evaluate(val_loader, loss_fn, false);
                    io::println("Epoch ", epoch, " | Train loss: ", train_loss,
                                " | Val loss: ", eval_result.first());
                }
            }
        }
    }

    void set_l2_lambda(T lambda) { l2_lambda_ = lambda; }

private:
    Module<T, Alloc>* model_;
    optim::Optimizer<T, Alloc>* optimizer_;
    T l2_lambda_;
};

} // namespace my::nn

#endif // NN_TRAINER_HPP
