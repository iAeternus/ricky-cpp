#include "linear.hpp"
#include "activations.hpp"
#include "loss.hpp"
#include "adam.hpp"
#include "dataset.hpp"
#include "dataloader.hpp"
#include "printer.hpp"

using namespace my;
using Tensor = nn::Tensor<f32>;

namespace config {

fs::PathBuf data_path = fs::PathBuf("examples/data/regression.csv");
fs::PathBuf model_path = fs::PathBuf("examples/models/regression_model.bin");

constexpr i32 epochs = 500;
constexpr i32 batch_size = 32;
constexpr f32 lr = 0.01f;
constexpr f32 weight_decay = 1e-4f;
constexpr usize n_features = 1;

} // namespace config

class RegModel : public nn::Module<f32> {
public:
    nn::Linear<f32> fc1;
    nn::Linear<f32> fc2;
    nn::Linear<f32> fc3;
    nn::ReLU<f32> relu1, relu2;

    RegModel() :
            fc1(nn::linear<f32>(1, 16)),
            fc2(nn::linear<f32>(16, 16)),
            fc3(nn::linear<f32>(16, 1)) {
        register_module(&fc1);
        register_module(&fc2);
        register_module(&fc3);
    }

    Tensor forward(const Tensor& x) override {
        auto z1 = fc1.forward(x);
        auto a1 = relu1.forward(z1);
        auto z2 = fc2.forward(a1);
        auto a2 = relu2.forward(z2);
        return fc3.forward(a2);
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

static void train_and_save(RegModel& model,
                           nn::data::CSVDataset<f32>& dataset,
                           nn::MSELoss<f32>& loss_fn) {
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
}

int main(int argc, char* argv[]) {
    bool train = argc > 1 && std::strcmp(argv[1], "-t") == 0;

    auto dataset = nn::data::CSVDataset<f32>(config::data_path, config::n_features);
    RegModel model;
    nn::MSELoss<f32> loss_fn("mean", config::weight_decay);

    if (train) {
        io::println("MLP Regression: y = sin(2pi*x) + noise");
        train_and_save(model, dataset, loss_fn);
        io::println("Model saved to ", config::model_path);
    } else {
        io::println("MLP Regression: Inference");
        model.load_params(config::model_path);
        auto test_loader = nn::data::DataLoader<f32>(dataset.test_set(), config::batch_size, false);
        auto [test_loss, _] = evaluate(model, test_loader, loss_fn);
        io::println("Test loss: ", test_loss);
    }

    io::println("Done.");
    return 0;
}
