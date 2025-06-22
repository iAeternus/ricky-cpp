### C++机器学习库详细设计文档

#### 项目目录结构
```
mlib/
├── include/                 # 公共头文件
│   ├── mlib/
│   │   ├── core/            # 核心基础设施
│   │   │   ├── tensor.h     # 张量实现
│   │   │   ├── autograd.h   # 自动微分引擎
│   │   │   └── allocator.h  # 内存分配器
│   │   ├── data/            # 数据处理
│   │   │   ├── dataset.h    # 数据集接口
│   │   │   └── loader.h     # 数据加载器
│   │   ├── models/          # 模型实现
│   │   │   ├── layer.h      # 层接口
│   │   │   ├── linear.h     # 线性层
│   │   │   └── sequential.h # 序列容器
│   │   ├── algorithms/      # 训练算法
│   │   │   ├── optimizer.h  # 优化器接口
│   │   │   ├── loss.h       # 损失函数
│   │   │   └── metrics.h    # 评估指标
│   │   └── utils/           # 实用工具
│   │       ├── serializer.h # 模型序列化
│   │       └── logger.h     # 日志系统
├── src/                     # 实现文件
│   ├── core/
│   ├── data/
│   ├── models/
│   ├── algorithms/
│   └── utils/
├── tests/                   # 单元测试
│   ├── test_tensor.cpp
│   ├── test_autograd.cpp
│   ├── test_linear.cpp
│   └── ...
└── CMakeLists.txt           # 构建配置
```

---

### 核心模块设计

#### **1. Tensor 类** (core/tensor.h)
```cpp
/**
 * @class Tensor
 * @brief 多维数组容器，支持自动微分
 * 
 * 张量是库中的基本数据类型，存储多维数据并记录计算历史
 * 支持CPU/GPU内存分配（通过Allocator策略类）
 */
class Tensor {
public:
    /**
     * @brief 构造函数
     * @param shape 张量维度 (e.g., {3, 224, 224})
     * @param requires_grad 是否计算梯度 (default=false)
     * @param allocator 内存分配器 (default=CPUPinnedAllocator)
     */
    Tensor(std::vector<size_t> shape, 
           bool requires_grad = false,
           Allocator* allocator = nullptr);
    
    /**
     * @brief 矩阵乘法
     * @param other 右乘张量
     * @return 矩阵乘积结果
     * @note 自动注册到计算图，支持自动微分
     * @throws std::invalid_argument 维度不匹配
     */
    Tensor matmul(const Tensor& other) const;
    
    /**
     * @brief 启动反向传播计算梯度
     * @param grad 反向传播的起始梯度 (默认单位张量)
     */
    void backward(const Tensor& grad = Tensor()) const;
    
    /**
     * @brief 获取梯度张量
     * @return 梯度张量引用
     * @throws std::runtime_error 未计算梯度
     */
    Tensor& grad() const;
    
    /**
     * @brief 重置梯度为0
     */
    void zero_grad() const;
    
    // 其他核心方法
    Tensor operator+(const Tensor& other) const;
    void reshape(const std::vector<size_t>& new_shape);
    float item() const; // 标量取值
    std::string to_string() const; // 调试输出
};
```

---

#### **2. Autograd 引擎** (core/autograd.h)
```cpp
/**
 * @class Function
 * @brief 计算图节点基类，实现前向/反向传播
 */
class Function {
public:
    /**
     * @brief 前向传播计算
     * @param inputs 输入张量列表
     * @return 输出张量列表
     */
    virtual std::vector<Tensor> forward(const std::vector<Tensor>& inputs) = 0;
    
    /**
     * @brief 反向传播梯度计算
     * @param grad_outputs 输出梯度列表
     * @return 输入梯度列表
     */
    virtual std::vector<Tensor> backward(const std::vector<Tensor>& grad_outputs) = 0;
    
    virtual ~Function() = default;
};

// 具体实现示例：矩阵乘法操作
class MatmulFunction : public Function {
public:
    std::vector<Tensor> forward(const std::vector<Tensor>& inputs) override;
    std::vector<Tensor> backward(const std::vector<Tensor>& grad_outputs) override;
};

// 自动微分引擎管理
class AutogradEngine {
public:
    /**
     * @brief 执行反向传播
     * @param root 计算图根节点
     */
    static void execute_backward(const Tensor& root);
};
```

---

### 模型模块设计

#### **3. Layer 接口** (models/layer.h)
```cpp
/**
 * @class Layer
 * @brief 神经网络层抽象接口
 */
class Layer {
public:
    /**
     * @brief 前向传播
     * @param input 输入张量
     * @return 输出张量
     */
    virtual Tensor forward(const Tensor& input) = 0;
    
    /**
     * @brief 获取可训练参数
     * @return 参数张量列表
     */
    virtual std::vector<Tensor> parameters() const = 0;
    
    /**
     * @brief 设置训练/推理模式
     * @param is_training true=训练模式, false=推理模式
     */
    virtual void train(bool is_training) = 0;
    
    virtual ~Layer() = default;
};
```

#### **4. Linear 层** (models/linear.h)
```cpp
/**
 * @class Linear
 * @brief 全连接层 y = xW^T + b
 */
class Linear : public Layer {
public:
    /**
     * @brief 构造函数
     * @param in_features 输入特征维度
     * @param out_features 输出特征维度
     * @param bias 是否包含偏置项 (default=true)
     */
    Linear(size_t in_features, size_t out_features, bool bias = true);
    
    Tensor forward(const Tensor& input) override;
    std::vector<Tensor> parameters() const override;
    void train(bool is_training) override;
    
private:
    Tensor weight;  // 形状 [out_features, in_features]
    Tensor bias;    // 形状 [out_features]
    bool use_bias;
};
```

---

### 算法模块设计

#### **5. Optimizer 接口** (algorithms/optimizer.h)
```cpp
/**
 * @class Optimizer
 * @brief 优化器抽象接口
 */
class Optimizer {
public:
    /**
     * @brief 构造函数
     * @param parameters 待优化参数列表
     * @param lr 学习率 (default=0.01)
     */
    Optimizer(std::vector<Tensor> parameters, float lr = 0.01f);
    
    /**
     * @brief 执行优化步骤
     */
    virtual void step() = 0;
    
    /**
     * @brief 重置参数梯度
     */
    virtual void zero_grad();
    
    virtual ~Optimizer() = default;

protected:
    std::vector<Tensor> parameters_;
    float learning_rate_;
};

/**
 * @class SGD
 * @brief 随机梯度下降优化器
 */
class SGD : public Optimizer {
public:
    using Optimizer::Optimizer;
    
    /**
     * @brief 执行SGD更新
     * @param momentum 动量因子 (default=0)
     * @param weight_decay L2正则化系数 (default=0)
     */
    void step() override;
    
    // 可选参数
    void configure(float momentum, float weight_decay);
};
```

#### **6. Loss 函数** (algorithms/loss.h)
```cpp
/**
 * @class MSELoss
 * @brief 均方误差损失函数
 */
class MSELoss {
public:
    /**
     * @brief 计算损失
     * @param input 模型输出
     * @param target 目标值
     * @return 损失张量
     */
    Tensor operator()(const Tensor& input, const Tensor& target) const;
};
```

---

### 工具模块设计

#### **7. 模型序列化** (utils/serializer.h)
```cpp
/**
 * @class ModelSerializer
 * @brief 模型序列化工具
 */
class ModelSerializer {
public:
    /**
     * @brief 保存模型到文件
     * @param model 模型对象
     * @param path 文件路径
     * @param format 序列化格式 (BINARY, JSON, ONNX)
     */
    static void save(const Layer& model, 
                    const std::string& path, 
                    SerializationFormat format = BINARY);
    
    /**
     * @brief 从文件加载模型
     * @param path 文件路径
     * @return 模型智能指针
     */
    static std::unique_ptr<Layer> load(const std::string& path);
};
```

---

### 测试用例设计

#### **张量运算测试** (tests/test_tensor.cpp)
```cpp
TEST_F(TensorTest, MatmulCorrectness) {
    Tensor a = {{1, 2}, {3, 4}}; // 2x2
    Tensor b = {{5, 6}, {7, 8}}; // 2x2
    Tensor c = a.matmul(b);
    
    ASSERT_EQ(c.shape(), std::vector<size_t>({2, 2}));
    EXPECT_FLOAT_EQ(c(0, 0).item(), 19);
    EXPECT_FLOAT_EQ(c(1, 1).item(), 50);
}

TEST_F(TensorTest, AutogradLinearChain) {
    Tensor x(2.0f, true); // requires_grad=true
    Tensor y = x * x * 3; // y = 3x^2
    y.backward();
    
    // dy/dx = 6x = 12
    ASSERT_TRUE(x.grad().defined());
    EXPECT_FLOAT_EQ(x.grad().item(), 12.0f);
}
```

#### **模型训练测试** (tests/test_linear.cpp)
```cpp
TEST_F(LinearModelTest, ConvergenceOnLinearData) {
    // 生成数据集 y = 2x + 1 + noise
    auto [train_x, train_y] = generate_linear_data(100, 2.0, 1.0, 0.1);
    
    // 构建模型
    Linear model(1, 1);
    MSELoss loss;
    SGD optimizer(model.parameters(), 0.01);
    
    // 训练循环
    for (int epoch = 0; epoch < 100; ++epoch) {
        optimizer.zero_grad();
        Tensor pred = model.forward(train_x);
        Tensor l = loss(pred, train_y);
        l.backward();
        optimizer.step();
    }
    
    // 验证参数收敛
    auto params = model.parameters();
    EXPECT_NEAR(params[0].item(), 2.0, 0.15); // weight ≈ 2
    EXPECT_NEAR(params[1].item(), 1.0, 0.15); // bias ≈ 1
}
```

#### **模型持久化测试** (tests/test_serialization.cpp)
```cpp
TEST_F(SerializationTest, SaveAndLoadModel) {
    // 创建并训练模型
    Linear original(10, 1);
    train_model(original); // 训练逻辑
    
    // 保存模型
    ModelSerializer::save(original, "test_model.bin");
    
    // 加载模型
    auto loaded = ModelSerializer::load("test_model.bin");
    auto* loaded_linear = dynamic_cast<Linear*>(loaded.get());
    
    // 验证参数一致性
    auto orig_params = original.parameters();
    auto loaded_params = loaded_linear->parameters();
    
    for (size_t i = 0; i < orig_params.size(); ++i) {
        EXPECT_TRUE(orig_params[i].allclose(loaded_params[i], 1e-5));
    }
}
```

---

### 扩展性设计要点

1. **多设备支持**
```cpp
// 在Tensor中增加设备上下文
class Tensor {
    // ...
    Device device() const;
    Tensor to(Device device) const;
};

// 设备类型枚举
enum class Device {
    CPU,
    CUDA,
    ROCm
};
```

2. **神经网络扩展**
```cpp
// 序列容器模型
class Sequential : public Layer {
public:
    void add_layer(std::shared_ptr<Layer> layer);
    Tensor forward(const Tensor& input) override;
    
private:
    std::vector<std::shared_ptr<Layer>> layers;
};

// 卷积层实现
class Conv2d : public Layer {
public:
    Conv2d(int in_channels, int out_channels, int kernel_size);
    // ... 实现细节
};
```

3. **分布式训练**
```cpp
class DistributedOptimizer : public Optimizer {
public:
    void step() override;
    void synchronize_parameters();
    
private:
    std::unique_ptr<DistributedBackend> backend;
};
```

4. **混合精度训练**
```cpp
class AMPContext {
public:
    static void enable(bool enabled);
    static Tensor autocast(const Tensor& input);
};
```

---

### 开发路线图

| 阶段        | 目标                      | 预计时间 |
| ----------- | ------------------------- | -------- |
| 1. 核心引擎 | Tensor+Autograd+Allocator | 4周      |
| 2. 基础模型 | Linear+Sequential+优化器  | 3周      |
| 3. 数据管道 | Dataset+Loader+预处理     | 2周      |
| 4. 生产化   | 序列化+日志+并发          | 3周      |
| 5. 神经网络 | CNN+RNN+Transformer       | 8周      |
| 6. 高级特性 | 分布式+混合精度           | 6周      |

---

### 关键注意事项

1. **内存管理**
   - 使用`std::shared_ptr`自定义删除器管理GPU内存
   - 实现内存池减少分配开销

2. **性能优化**
   - 使用表达式模板避免临时对象
   - 实现算子融合优化
   ```cpp
   // 表达式模板示例
   template<typename E1, typename E2>
   class AddExpr {
       // 延迟计算逻辑
   };
   ```

3. **API设计**
   - 保持PyTorch-like API风格
   - 提供Python绑定接口(PyBind11)

4. **跨平台支持**
   - 使用CMake条件编译
   - 抽象设备相关代码
   ```cmake
   option(USE_CUDA "Enable CUDA support" OFF)
   if(USE_CUDA)
       add_definitions(-DMLIB_USE_CUDA)
   endif()
   ```

5. **测试覆盖率**
   - 核心模块100%覆盖率
   - 定期性能基准测试
   - 模型收敛性验证

本设计文档提供了完整的库架构和实现规范，后续开发应遵循：
1. 先实现核心引擎（Tensor+Autograd）
2. 通过严格测试确保数值正确性
3. 逐步扩展高级功能
4. 保持API向后兼容
5. 定期进行性能优化