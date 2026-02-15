#include "test_registry.hpp"
#include "printer.hpp"
#include "color.hpp"
#include "timer.hpp"
#include "my_exception.hpp"

#include <cstring>

namespace my::test {

static io::ColorPrinter g_passed{stdout, color::Color::GREEN};
static io::ColorPrinter g_failed{stdout, color::Color::RED};
static io::ColorPrinter g_info{stdout, color::Color::AQUA};

Registry& Registry::instance() {
    static Registry inst{};
    return inst;
}

void Registry::add_case(const CString& suite, const CString& name, Runnable fn) {
    cases_.push(TestCase{suite, name, std::move(fn)});
}

void Registry::add_bench(const CString& suite, const CString& name, Runnable fn, const BenchConfig& cfg) {
    benches_.push(BenchCase{suite, name, std::move(fn), cfg});
}

const util::Vec<TestCase>& Registry::cases() const {
    return cases_;
}

const util::Vec<BenchCase>& Registry::benches() const {
    return benches_;
}

int run_all() {
    const auto& cases = Registry::instance().cases();
    if (cases.is_empty()) {
        g_info("[TEST] 未发现任何单元测试用例");
        return 0;
    }

    usize failed = 0;
    CString last_suite;
    util::Timer_us timer;

    for (const auto& item : cases) {
        if (last_suite != item.suite) {
            last_suite = item.suite;
            io::println(std::format("================= {} =================", last_suite));
        }

        try {
            auto cost = timer(item.fn);
            g_passed(std::format("[PASS] {} ({}us)", item.name, cost));
        } catch (const Exception& ex) {
            ++failed;
            g_failed(std::format("[FAIL] {}\nCaused by: {}", item.name, ex.what()));
        } catch (const std::exception& ex) {
            ++failed;
            g_failed(std::format("[FAIL] {}\nCaused by: {}", item.name, ex.what()));
        } catch (...) {
            ++failed;
            g_failed(std::format("[FAIL] {}\nCaused by: Unknown exception", item.name));
        }
    }

    io::println(std::format("[TEST] Total: {}, Failures: {}", cases.len(), failed));
    return failed == 0 ? 0 : 1;
}

static double avg_of(const util::Vec<u64>& values) {
    if (values.is_empty()) return 0.0;
    long double sum = 0.0;
    for (auto v : values) sum += static_cast<long double>(v);
    return static_cast<double>(sum / static_cast<long double>(values.len()));
}

static const char* pick_unit(u64 ns) {
    if (ns >= 1000000000ULL) return "s";
    if (ns >= 1000000ULL) return "ms";
    if (ns >= 1000ULL) return "us";
    return "ns";
}

static std::string format_time_ns(u64 ns, const char* unit) {
    if (std::strcmp(unit, "s") == 0) {
        return std::format("{:.2f}", static_cast<double>(ns) / 1e9);
    }
    if (std::strcmp(unit, "ms") == 0) {
        return std::format("{:.2f}", static_cast<double>(ns) / 1e6);
    }
    if (std::strcmp(unit, "us") == 0) {
        return std::format("{:.2f}", static_cast<double>(ns) / 1e3);
    }
    return std::format("{}", ns);
}

static std::string color_line(const char* color, const std::string& line) {
    return std::format("{}{}{}", color, line, color::Color::CLOSE);
}

int run_benchmarks() {
    const auto& benches = Registry::instance().benches();
    if (benches.is_empty()) {
        g_info("[BENCH] 未发现任何基准测试用例");
        return 0;
    }

    CString last_suite;
    util::Timer_ns timer;

    for (const auto& item : benches) {
        if (last_suite != item.suite) {
            last_suite = item.suite;
            io::println(std::format("================= {} =================", last_suite));
        }

        // 预热
        for (u32 i = 0; i < item.cfg.warmup; ++i) {
            item.fn();
        }

        util::Vec<u64> per_iter_ns;
        per_iter_ns.reserve(item.cfg.repeats);

        for (u32 r = 0; r < item.cfg.repeats; ++r) {
            timer.start();
            for (u32 i = 0; i < item.cfg.iters; ++i) {
                item.fn();
            }
            auto total_ns = static_cast<u64>(timer.end());
            auto per_iter = item.cfg.iters == 0 ? 0ULL : total_ns / item.cfg.iters;
            per_iter_ns.push(per_iter);
        }

        u64 min_v = per_iter_ns[0];
        u64 max_v = per_iter_ns[0];
        for (auto v : per_iter_ns) {
            if (v < min_v) min_v = v;
            if (v > max_v) max_v = v;
        }
        double avg_v = avg_of(per_iter_ns);

        g_info(std::format("[BENCH] {} | iters={} repeats={} warmup={}",
                           item.name, item.cfg.iters, item.cfg.repeats, item.cfg.warmup));
        const auto unit = pick_unit(static_cast<u64>(avg_v));
        const auto avg_str = format_time_ns(static_cast<u64>(avg_v), unit);
        const auto min_str = format_time_ns(min_v, unit);
        const auto max_str = format_time_ns(max_v, unit);
        io::println(color_line(color::Color::YELLOW,
                               std::format("         time/iter: min={}{} avg={}{} max={}{}",
                                           min_str, unit, avg_str, unit, max_str, unit)));
    }

    return 0;
}

} // namespace my::test
