#ifndef TEST_REGISTRY_HPP
#define TEST_REGISTRY_HPP

#include "my_types.hpp"
#include "my_func.hpp"
#include "cstring.hpp"
#include "vec.hpp"
#include "object.hpp"

namespace my::test {

/**
 * @brief 基准测试配置
 */
struct BenchConfig {
    u32 warmup{10};
    u32 iters{100};
    u32 repeats{5};
};

/**
 * @brief 单元测试用例
 */
struct TestCase {
    CString suite;
    CString name;
    Runnable fn;
};

/**
 * @brief 基准测试用例
 */
struct BenchCase {
    CString suite;
    CString name;
    Runnable fn;
    BenchConfig cfg;
};

/**
 * @brief 注册项（用于宏批量注册）
 */
struct TestRegItem {
    const char* name;
    Runnable fn;
};

/**
 * @brief 基准注册项（用于宏批量注册）
 */
struct BenchRegItem {
    const char* name;
    Runnable fn;
    BenchConfig cfg;
};

/**
 * @brief 测试注册表（单例）
 */
class Registry : public Object<Registry> {
public:
    static Registry& instance();

    void add_case(const CString& suite, const CString& name, Runnable fn);
    void add_bench(const CString& suite, const CString& name, Runnable fn, const BenchConfig& cfg);

    const util::Vec<TestCase>& cases() const;
    const util::Vec<BenchCase>& benches() const;

private:
    util::Vec<TestCase> cases_{};
    util::Vec<BenchCase> benches_{};
};

/**
 * @brief 注册单元测试
 */
inline void register_case(const char* suite, const char* name, Runnable fn) {
    Registry::instance().add_case(CString{suite}, CString{name}, std::move(fn));
}

/**
 * @brief 注册基准测试
 */
inline void register_bench(const char* suite, const char* name, Runnable fn, const BenchConfig& cfg = {}) {
    Registry::instance().add_bench(CString{suite}, CString{name}, std::move(fn), cfg);
}

/**
 * @brief 批量注册单元测试
 */
inline void register_case_items(const char* suite, std::initializer_list<TestRegItem> items) {
    for (const auto& it : items) {
        register_case(suite, it.name, it.fn);
    }
}

/**
 * @brief 批量注册基准测试
 */
inline void register_bench_items(const char* suite, std::initializer_list<BenchRegItem> items) {
    for (const auto& it : items) {
        register_bench(suite, it.name, it.fn, it.cfg);
    }
}

/**
 * @brief 运行所有单元测试
 * @return 0=全部通过，非0=存在失败
 */
int run_all();

/**
 * @brief 运行所有基准测试
 * @return 0=正常完成，非0=存在失败
 */
int run_benchmarks();

} // namespace my::test

// ---------------------- 少量宏（可选）----------------------

/**
 * @brief 声明当前文件测试套件名
 */
#ifdef GROUP_NAME
#undef GROUP_NAME
#endif
#ifdef BENCH_NAME
#undef BENCH_NAME
#endif
#define TEST_SUITE(name) static constexpr const char* MY_TEST_SUITE = name;

/**
 * @brief 测试组名称（单元）
 */
#define GROUP_NAME(name) static constexpr const char* MY_TEST_SUITE = name;

/**
 * @brief 基准组名称
 */
#define BENCH_NAME(name) static constexpr const char* MY_TEST_SUITE = name;

/**
 * @brief 单项注册（保留）
 */
#define TEST_CASE(name, fn) ::my::test::register_case(MY_TEST_SUITE, name, fn)
#define BENCH_CASE(name, fn) ::my::test::register_bench(MY_TEST_SUITE, name, fn)

/**
 * @brief 基准测试配置
 */
#define BENCH_CONFIG(warmup, iters, repeats) ::my::test::BenchConfig{(warmup), (iters), (repeats)}

/**
 * @brief 批量注册（推荐）
 */
#define REGISTER_UNIT_TESTS(...) \
    namespace { \
    struct AutoRegisterUnit { \
        AutoRegisterUnit() { \
            ::my::test::register_case_items(MY_TEST_SUITE, { __VA_ARGS__ }); \
        } \
    }; \
    static AutoRegisterUnit g_auto_register_unit; \
    }

#define REGISTER_BENCH_TESTS(...) \
    namespace { \
    struct AutoRegisterBench { \
        AutoRegisterBench() { \
            ::my::test::register_bench_items(MY_TEST_SUITE, { __VA_ARGS__ }); \
        } \
    }; \
    static AutoRegisterBench g_auto_register_bench; \
    }

/**
 * @brief 便捷项构造
 */
#define UNIT_TEST_ITEM(fn) ::my::test::TestRegItem{#fn, fn}
#define BENCH_TEST_ITEM(fn) ::my::test::BenchRegItem{#fn, fn, {}}
#define BENCH_TEST_ITEM_CFG(fn, cfg) ::my::test::BenchRegItem{#fn, fn, cfg}

#endif // TEST_REGISTRY_HPP
