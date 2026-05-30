/**
 * @brief MLP 回归示例：y = sin(2πx) + 噪声
 * @author opencode
 * @date 2026/05/30
 * @version 1.0
 *
 * 演示：数据生成 → CSV 保存/读取 → 模型定义 → 训练 → 评估
 */
#include "nn/linear.hpp"
#include "nn/activations.hpp"
#include "nn/loss.hpp"
#include "nn/optim/adam.hpp"
#include "fs/file.hpp"
#include "io/printer.hpp"
#include "math/math_utils.hpp"

#include <cmath>
#include <sstream>

using namespace my;
using Tensor = nn::Tensor<f32>;
using Shape  = typename Tensor::Shape;

constexpr usize  N_SAMPLES     = 200;
constexpr i32    N_EPOCHS      = 500;
constexpr f32    LEARNING_RATE = 0.01f;
const char* const DATA_PATH    = "regression_data.csv";

class Model : public nn::Module<f32> {
public:
    nn::Linear<f32> fc1, fc2, fc3;
    nn::ReLU<f32>   relu1, relu2;

    Model() : fc1(1, 64), fc2(64, 64), fc3(64, 1) {
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

void generate_data(Tensor& x, Tensor& y) {
    x = Tensor(Shape{N_SAMPLES, 1}, 0.0f);
    y = Tensor(Shape{N_SAMPLES, 1}, 0.0f);

    for (usize i = 0; i < N_SAMPLES; ++i) {
        f32 val = -3.0f + 6.0f * static_cast<f32>(i) / (N_SAMPLES - 1);
        x(i, 0) = val;
        y(i, 0) = std::sin(2.0f * static_cast<f32>(math::PI) * val)
                + 0.1f * Tensor::randn({1}).data()[0];
    }
}

void save_csv(const Tensor& x, const Tensor& y, const char* path) {
    auto file = fs::File::create(path);
    for (usize i = 0; i < N_SAMPLES; ++i) {
        std::stringstream ss;
        ss << x(i, 0) << "," << y(i, 0) << "\n";
        file.write(CString(ss.str()));
    }
    file.flush();
}

std::pair<Tensor, Tensor> load_csv(const char* path) {
    auto file = fs::File::open(path);
    auto content = file.read_all();
    auto data_cstr = content.as_cstr();

    Tensor x(Shape{N_SAMPLES, 1}, 0.0f);
    Tensor y(Shape{N_SAMPLES, 1}, 0.0f);

    std::stringstream ss(data_cstr);
    std::string line;
    usize idx = 0;
    while (std::getline(ss, line) && idx < N_SAMPLES) {
        auto comma = line.find(',');
        x(idx, 0) = std::stof(line.substr(0, comma));
        y(idx, 0) = std::stof(line.substr(comma + 1));
        ++idx;
    }

    return {std::move(x), std::move(y)};
}

int main() {
    io::println("=== MLP Regression: y = sin(2\u03c0x) + noise ===");

    Tensor x, y;
    generate_data(x, y);
    save_csv(x, y, DATA_PATH);
    io::println("Data saved to ", DATA_PATH);

    auto [data_x, data_y] = load_csv(DATA_PATH);
    io::println("Loaded ", data_x.shape()[0], " samples");

    Model model;
    nn::MSELoss<f32> loss_fn;
    nn::optim::Adam<f32> optimizer(model.parameters(), LEARNING_RATE);

    io::println("\nTraining...");
    for (i32 epoch = 0; epoch < N_EPOCHS; ++epoch) {
        optimizer.zero_grad();
        auto pred = model.forward(data_x);
        auto loss = loss_fn.forward(pred, data_y);
        loss.backward();
        optimizer.step();

        if (epoch % 100 == 0 || epoch == N_EPOCHS - 1) {
            io::println("Epoch ", epoch, " | Loss: ", loss.data()[0]);
        }
    }

    auto final_pred = model.forward(data_x);
    auto final_loss = loss_fn.forward(final_pred, data_y);
    io::println("\nFinal loss: ", final_loss.data()[0]);

    io::println("\nSample predictions:");
    for (usize i = 0; i < N_SAMPLES; i += N_SAMPLES / 10) {
        io::println("  x: ", data_x(i, 0), " | true: ", data_y(i, 0), " | pred: ", final_pred(i, 0));
    }

    io::println("Done.");
    return 0;
}
