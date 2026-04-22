#include <cstdlib>
#include <iostream>

#include "latency_stats.h"

namespace {

bool check_eq(const char* label, const std::int64_t actual, const std::int64_t expected) {
    if (actual == expected) {
        return true;
    }

    std::cerr << label << " expected=" << expected << " actual=" << actual << '\n';
    return false;
}

bool check_eq_size(const char* label, const std::size_t actual, const std::size_t expected) {
    if (actual == expected) {
        return true;
    }

    std::cerr << label << " expected=" << expected << " actual=" << actual << '\n';
    return false;
}

bool test_empty_summary() {
    llt::LatencyStats stats;
    const llt::LatencySummary summary = stats.summarize();

    return check_eq_size("empty.count", summary.count, 0) &&
        check_eq("empty.min_ns", summary.min_ns, 0) &&
        check_eq("empty.mean_ns", summary.mean_ns, 0) &&
        check_eq("empty.p50_ns", summary.p50_ns, 0) &&
        check_eq("empty.p99_ns", summary.p99_ns, 0) &&
        check_eq("empty.p99_9_ns", summary.p99_9_ns, 0) &&
        check_eq("empty.tail_mean_99_ns", summary.tail_mean_99_ns, 0) &&
        check_eq("empty.max_ns", summary.max_ns, 0);
}

bool test_percentiles_and_tail_mean() {
    llt::LatencyStats stats;
    stats.reserve(1000);
    for (std::int64_t sample = 1; sample <= 1000; ++sample) {
        stats.record(sample);
    }

    const llt::LatencySummary summary = stats.summarize();

    return check_eq_size("summary.count", summary.count, 1000) &&
        check_eq("summary.min_ns", summary.min_ns, 1) &&
        check_eq("summary.mean_ns", summary.mean_ns, 500) &&
        check_eq("summary.p50_ns", summary.p50_ns, 500) &&
        check_eq("summary.p99_ns", summary.p99_ns, 990) &&
        check_eq("summary.p99_9_ns", summary.p99_9_ns, 999) &&
        check_eq("summary.tail_mean_99_ns", summary.tail_mean_99_ns, 995) &&
        check_eq("summary.max_ns", summary.max_ns, 1000);
}

bool test_tail_mean_rounds_up_to_one_sample() {
    llt::LatencyStats stats;
    stats.record(5);
    stats.record(10);
    stats.record(20);

    const llt::LatencySummary summary = stats.summarize();

    return check_eq_size("small.count", summary.count, 3) &&
        check_eq("small.tail_mean_99_ns", summary.tail_mean_99_ns, 20) &&
        check_eq("small.p99_9_ns", summary.p99_9_ns, 10);
}

}  // namespace

int main() {
    const bool ok =
        test_empty_summary() &&
        test_percentiles_and_tail_mean() &&
        test_tail_mean_rounds_up_to_one_sample();

    if (!ok) {
        return EXIT_FAILURE;
    }

    std::cout << "latency_stats_tests: all tests passed\n";
    return EXIT_SUCCESS;
}
