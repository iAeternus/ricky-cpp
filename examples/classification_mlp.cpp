#include "nn/linear.hpp"
#include "nn/activations.hpp"
#include "nn/loss.hpp"
#include "nn/optim/adam.hpp"
#include "nn/data/dataset.hpp"
#include "nn/data/dataloader.hpp"
#include "io/printer.hpp"
#include <cstring>

using namespace my;
using Tensor = nn::Tensor<f32>;

namespace config {

constexpr i32 epochs = 300;
constexpr i32 batch_size = 16;
constexpr f32 lr = 0.01f;
constexpr f32 weight_decay = 1e-4f;
constexpr const char* data_path = "examples/data/classification.csv";
constexpr const char* model_path = "examples/models/classification_model.bin";
constexpr usize n_features = 4;
constexpr usize n_classes = 3;

} // namespace config

class ClsModel : public nn::Module<f32> {
public:
    nn::Linear<f32> fc1{4, 16}, fc2{16, 16}, fc3{16, 3};
    nn::ReLU<f32> relu1, relu2;

    ClsModel() {
        register_module(&fc1);
        register_module(&fc2);
        register_module(&fc3);
    }

    Tensor forward(const Tensor& x) override {
        auto h = relu1.forward(fc1.forward(x));
        h = relu2.forward(fc2.forward(h));
        return fc3.forward(h);
    }
};

static f32 train_one_epoch(ClsModel& model, nn::optim::Adam<f32>& opt,
                           nn::data::DataLoader<f32>& loader,
                           nn::CrossEntropyLoss<f32>& loss_fn) {
    f32 total_loss = 0;
    usize batches = 0;
    loader.reset();
    while (loader.has_next()) {
        auto batch = loader.next_batch();
        opt.zero_grad();
        auto pred = model.forward(batch.first());
        auto loss = loss_fn.forward(pred, batch.second());
        loss.backward();
        opt.step();
        total_loss += loss.data()[0];
        ++batches;
    }
    return total_loss / static_cast<f32>(batches);
}

static Pair<f32, f32> evaluate(ClsModel& model,
                               nn::data::DataLoader<f32>& loader,
                               nn::CrossEntropyLoss<f32>& loss_fn) {
    f32 total_loss = 0;
    usize correct = 0;
    usize total = 0;
    loader.reset();
    while (loader.has_next()) {
        auto batch = loader.next_batch();
        auto& batch_x = batch.first();
        auto& batch_y = batch.second();
        auto pred = model.forward(batch_x);
        auto loss = loss_fn.forward(pred, batch_y);
        total_loss += loss.data()[0];

        usize bs = batch_x.shape()[0];
        usize nc = config::n_classes;
        for (usize i = 0; i < bs; ++i) {
            f32 max_val = pred(i, 0);
            usize pred_class = 0;
            for (usize j = 1; j < nc; ++j) {
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
        total += bs;
    }

    f32 avg_loss = total_loss / static_cast<f32>(loader.size() / loader.batch_size() + 1);
    f32 acc = static_cast<f32>(correct) / static_cast<f32>(total) * 100;
    return {avg_loss, acc};
}

int main(int argc, char* argv[]) {
    bool train = argc > 1 && std::strcmp(argv[1], "-t") == 0;

    auto dataset = nn::data::CSVDataset<f32>(config::data_path, config::n_features);
    ClsModel model;
    nn::CrossEntropyLoss<f32> loss_fn(config::weight_decay);

    if (train) {
        io::println("=== MLP Classification: Iris (3 classes) ===");
        loss_fn.set_params(model.parameters());
        nn::optim::Adam<f32> optimizer(model.parameters(), config::lr);
        auto train_loader = nn::data::DataLoader<f32>(dataset.train_set(), config::batch_size, true);
        auto val_loader = nn::data::DataLoader<f32>(dataset.val_set(), config::batch_size, false);

        for (i32 epoch = 0; epoch < config::epochs; ++epoch) {
            auto train_loss = train_one_epoch(model, optimizer, train_loader, loss_fn);
            if (epoch % 50 == 0 || epoch == config::epochs - 1) {
                auto [val_loss, val_acc] = evaluate(model, val_loader, loss_fn);
                io::println("Epoch ", epoch, " | Train loss: ", train_loss,
                            " | Val loss: ", val_loss, " | Val acc: ", val_acc, "%");
            }
        }

        model.save_params(config::model_path);
        io::println("Model saved to ", config::model_path);
    } else {
        io::println("=== MLP Classification: Inference ===");
        model.load_params(config::model_path);
        auto test_loader = nn::data::DataLoader<f32>(dataset.test_set(), config::batch_size, false);
        auto [test_loss, test_acc] = evaluate(model, test_loader, loss_fn);
        io::println("Test loss: ", test_loss, " | Test acc: ", test_acc, "%");
    }

    io::println("Done.");
    return 0;
}
