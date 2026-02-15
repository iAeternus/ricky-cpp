/**
 * @brief 测试入口
 */
#define TEST_MODE 1

#include "ricky_test.hpp"

auto main() -> int {
#if TEST_MODE == 0
    return my::test::run_benchmarks();
#else
    return my::test::run_all();
#endif
}