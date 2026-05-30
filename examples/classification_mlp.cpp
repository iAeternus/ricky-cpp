#include "nn/linear.hpp"
#include "nn/activations.hpp"
#include "nn/loss.hpp"
#include "nn/optim/adam.hpp"
#include "nn/trainer.hpp"
#include "nn/data/dataset.hpp"
#include "nn/data/dataloader.hpp"

using namespace my;
using Tensor = nn::Tensor<f32>;
using Shape = typename Tensor::Shape;

constexpr i32 N_EPOCHS = 300;

class ClsModel : public nn::Module<f32> {
public:
    nn::Linear<f32> fc1, fc2, fc3;
    nn::ReLU<f32> relu1, relu2;

    ClsModel() : fc1(4, 16), fc2(16, 16), fc3(16, 3) {
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

int main() {
    io::println("=== MLP Classification: Iris (3 classes) ===");

    auto dataset = nn::data::CSVDataset<f32>("examples/data/classification.csv", 4);
    io::println("Loaded ", dataset.train_set().size(), " train, ",
                dataset.val_set().size(), " val, ",
                dataset.test_set().size(), " test samples");

    ClsModel model;
    nn::CrossEntropyLoss<f32> loss_fn(1e-4f);
    loss_fn.set_params(model.parameters());
    nn::optim::Adam<f32> optimizer(model.parameters(), 0.01f);

    nn::Trainer<f32> trainer(model, optimizer);
    auto train_loader = nn::data::DataLoader<f32>(dataset.train_set(), 16, true);
    auto val_loader = nn::data::DataLoader<f32>(dataset.val_set(), 16, false);

    trainer.train(train_loader, val_loader, loss_fn, N_EPOCHS, true);

    auto test_loader = nn::data::DataLoader<f32>(dataset.test_set(), 16, false);
    auto [test_loss, test_acc] = trainer.evaluate(test_loader, loss_fn, true);
    io::println("\nTest loss: ", test_loss, " | Test acc: ", test_acc, "%");

    model.save_params("examples/models/classification_model.bin");
    io::println("Model saved to examples/models/classification_model.bin");

    io::println("Done.");
    return 0;
}
