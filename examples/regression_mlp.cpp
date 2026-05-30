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

constexpr i32 epochs = 500;
constexpr i32 batch_size = 32;
constexpr f32 lr = 0.01f;
constexpr f32 weight_decay = 1e-4f;
constexpr const char* data_path = "examples/data/regression.csv";
constexpr const char* model_path = "examples/models/regression_model.bin";
constexpr usize n_features = 1;

} // namespace config

class RegModel : public nn::Module<f32> {
public:
    nn::Linear<f32> fc1{1, 16}, fc2{16, 16}, fc3{16, 1};
    nn::ReLU<f32> relu1, relu2;

    RegModel() {
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

static f32 train_one_epoch(RegModel& model, nn::optim::Adam<f32>& opt,
                           nn::data::DataLoader<f32>& loader,
                           nn::MSELoss<f32>& loss_fn) {
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

static Pair<f32, f32> evaluate(RegModel& model,
                               nn::data::DataLoader<f32>& loader,
                               nn::MSELoss<f32>& loss_fn) {
    f32 total_loss = 0;
    loader.reset();
    while (loader.has_next()) {
        auto batch = loader.next_batch();
        auto pred = model.forward(batch.first());
        auto loss = loss_fn.forward(pred, batch.second());
        total_loss += loss.data()[0];
    }
    f32 avg = total_loss / static_cast<f32>(loader.size() / loader.batch_size() + 1);
    return {avg, 0};
}

int main(int argc, char* argv[]) {
    bool train = argc > 1 && std::strcmp(argv[1], "-t") == 0;

    auto dataset = nn::data::CSVDataset<f32>(config::data_path, config::n_features);
    RegModel model;
    nn::MSELoss<f32> loss_fn("mean", config::weight_decay);

    if (train) {
        io::println("=== MLP Regression: y = sin(2pi*x) + noise ===");
        loss_fn.set_params(model.parameters());
        nn::optim::Adam<f32> optimizer(model.parameters(), config::lr);
        auto train_loader = nn::data::DataLoader<f32>(dataset.train_set(), config::batch_size, true);
        auto val_loader = nn::data::DataLoader<f32>(dataset.val_set(), config::batch_size, false);

        for (i32 epoch = 0; epoch < config::epochs; ++epoch) {
            auto train_loss = train_one_epoch(model, optimizer, train_loader, loss_fn);
            if (epoch % 50 == 0 || epoch == config::epochs - 1) {
                auto [val_loss, _] = evaluate(model, val_loader, loss_fn);
                io::println("Epoch ", epoch, " | Train loss: ", train_loss, " | Val loss: ", val_loss);
            }
        }

        model.save_params(config::model_path);
        io::println("Model saved to ", config::model_path);
    } else {
        io::println("=== MLP Regression: Inference ===");
        model.load_params(config::model_path);
        auto test_loader = nn::data::DataLoader<f32>(dataset.test_set(), config::batch_size, false);
        auto [test_loss, _] = evaluate(model, test_loader, loss_fn);
        io::println("Test loss: ", test_loss);
    }

    io::println("Done.");
    return 0;
}
