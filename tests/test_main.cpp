/**
 * @brief 测试入口
 */
#include "ricky_test.hpp"

#include <cstring>

namespace {

bool is_bench_mode(int argc, char** argv) {
    for (int i = 1; i < argc; ++i) {
        const char* arg = argv[i];
        if (std::strcmp(arg, "--bench") == 0 || std::strcmp(arg, "-B") == 0) {
            return true;
        }
    }
    return false;
}

} // namespace

int main(int argc, char** argv) {
    if (is_bench_mode(argc, argv)) {
        return my::test::run_benchmarks();
    }
    return my::test::run_all();
}
