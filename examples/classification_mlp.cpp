/**
 * @brief MLP 分类示例：make_moons 二分类
 * @author opencode
 * @date 2026/05/30
 * @version 1.0
 *
 * 演示：数据生成（make_moons）→ CSV 保存/读取 → 模型定义 → 训练 → 精度评估
 */
#include "nn/linear.hpp"
#include "nn/activations.hpp"
#include "nn/loss.hpp"
#include "nn/functional.hpp"
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
constexpr f32    NOISE         = 0.15f;
const char* const DATA_PATH    = "classification_data.csv";

class Model : public nn::Module<f32> {
public:
    nn::Linear<f32> fc1, fc2, fc3;
    nn::ReLU<f32>   relu1, relu2;

    Model() : fc1(2, 32), fc2(32, 32), fc3(32, 2) {
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

void generate_moons(Tensor& x, Tensor& y) {
    usize n_out = N_SAMPLES / 2;
    usize n_in  = N_SAMPLES - n_out;

    x = Tensor(Shape{N_SAMPLES, 2}, 0.0f);
    y = Tensor(Shape{N_SAMPLES}, 0.0f);

    for (usize i = 0; i < n_out; ++i) {
        f32 t = static_cast<f32>(math::PI) * i / (n_out - 1);
        f32 nx = Tensor::randn({1}).data()[0] * NOISE;
        f32 ny = Tensor::randn({1}).data()[0] * NOISE;
        x(i, 0) = std::cos(t) + nx;
        x(i, 1) = std::sin(t) + ny;
        y(i) = 0.0f;
    }

    for (usize i = 0; i < n_in; ++i) {
        f32 t = static_cast<f32>(math::PI) * i / (n_in - 1);
        f32 nx = Tensor::randn({1}).data()[0] * NOISE;
        f32 ny = Tensor::randn({1}).data()[0] * NOISE;
        x(n_out + i, 0) = 1.0f - std::cos(t) + nx;
        x(n_out + i, 1) = 1.0f - std::sin(t) + ny;
        y(n_out + i) = 1.0f;
    }
}

void save_csv(const Tensor& x, const Tensor& y, const char* path) {
    auto file = fs::File::create(path);
    for (usize i = 0; i < N_SAMPLES; ++i) {
        std::stringstream ss;
        ss << x(i, 0) << "," << x(i, 1) << "," << static_cast<i32>(y(i)) << "\n";
        file.write(CString(ss.str()));
    }
    file.flush();
}

std::pair<Tensor, Tensor> load_csv(const char* path) {
    auto file = fs::File::open(path);
    auto content = file.read_all();
    auto data_cstr = content.as_cstr();

    Tensor x(Shape{N_SAMPLES, 2}, 0.0f);
    Tensor y(Shape{N_SAMPLES}, 0.0f);

    std::stringstream ss(data_cstr);
    std::string line;
    usize idx = 0;
    while (std::getline(ss, line) && idx < N_SAMPLES) {
        auto c1 = line.find(',');
        auto c2 = line.find(',', c1 + 1);
        x(idx, 0) = std::stof(line.substr(0, c1));
        x(idx, 1) = std::stof(line.substr(c1 + 1, c2 - c1 - 1));
        y(idx) = static_cast<f32>(std::stoi(line.substr(c2 + 1)));
        ++idx;
    }

    return {std::move(x), std::move(y)};
}

i32 compute_accuracy(const Tensor& pred_logits, const Tensor& target) {
    i32 correct = 0;
    usize batch = pred_logits.shape()[0];
    for (usize i = 0; i < batch; ++i) {
        f32 logit0 = pred_logits(i, 0);
        f32 logit1 = pred_logits(i, 1);
        i32 pred_class = (logit1 > logit0) ? 1 : 0;
        i32 true_class = static_cast<i32>(target(i));
        if (pred_class == true_class) ++correct;
    }
    return correct;
}

int main() {
    io::println("=== MLP Classification: make_moons ===");

    Tensor x, y;
    generate_moons(x, y);
    save_csv(x, y, DATA_PATH);
    io::println("Data saved to ", DATA_PATH);

    auto [data_x, data_y] = load_csv(DATA_PATH);
    usize n_total = data_x.shape()[0];
    io::println("Loaded ", n_total, " samples");

    Model model;
    nn::CrossEntropyLoss<f32> loss_fn;
    nn::optim::Adam<f32> optimizer(model.parameters(), LEARNING_RATE);

    io::println("\nTraining...");
    for (i32 epoch = 0; epoch < N_EPOCHS; ++epoch) {
        optimizer.zero_grad();
        auto logits = model.forward(data_x);
        auto loss = loss_fn.forward(logits, data_y);
        loss.backward();
        optimizer.step();

        if (epoch % 100 == 0 || epoch == N_EPOCHS - 1) {
            i32 correct = compute_accuracy(logits, data_y);
            io::println("Epoch ", epoch, " | Loss: ", loss.data()[0],
                        " | Acc: ", 100.0f * correct / n_total, "%");
        }
    }

    auto final_logits = model.forward(data_x);
    auto final_loss = loss_fn.forward(final_logits, data_y);
    i32 correct = compute_accuracy(final_logits, data_y);
    io::println("\nFinal loss: ", final_loss.data()[0]);
    io::println("Accuracy: ", 100.0f * correct / n_total, "% (", correct, "/", n_total, ")");

    io::println("\nSample predictions:");
    for (usize i = 0; i < n_total; i += n_total / 5) {
        f32 l0 = final_logits(i, 0);
        f32 l1 = final_logits(i, 1);
        i32 pred = (l1 > l0) ? 1 : 0;
        i32 true_label = static_cast<i32>(data_y(i));
        io::println("  (", data_x(i, 0), ", ", data_x(i, 1),
                    ") | true: ", true_label, " | pred: ", pred,
                    " | logits: (", l0, ", ", l1, ")");
    }

    io::println("Done.");
    return 0;
}
