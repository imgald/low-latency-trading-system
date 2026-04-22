#include "latency_stats.h"

#include <algorithm>
#include <ostream>

namespace llt {

void LatencyStats::reserve(const std::size_t capacity) {
    samples_.reserve(capacity);
}

void LatencyStats::record(const std::int64_t latency_ns) {
    samples_.push_back(latency_ns);
}

LatencySummary LatencyStats::summarize() const {
    if (samples_.empty()) {
        return LatencySummary{0, 0, 0, 0, 0, 0, 0, 0};
    }

    std::vector<std::int64_t> sorted = samples_;
    // Summaries are computed off the hot path, so we keep the per-sample recording logic minimal
    // and do the copy/sort work only when we explicitly print or inspect the distribution.
    std::sort(sorted.begin(), sorted.end());

    std::int64_t total_ns = 0;
    for (const std::int64_t sample : sorted) {
        total_ns += sample;
    }

    const std::size_t tail_count = tail_count_for_top_percent(sorted.size(), 1);
    const std::size_t tail_start = sorted.size() - tail_count;
    std::int64_t tail_total_ns = 0;
    for (std::size_t index = tail_start; index < sorted.size(); ++index) {
        tail_total_ns += sorted[index];
    }

    return LatencySummary{
        sorted.size(),
        sorted.front(),
        total_ns / static_cast<std::int64_t>(sorted.size()),
        sorted[percentile_index_permille(sorted.size(), 500)],
        sorted[percentile_index_permille(sorted.size(), 990)],
        sorted[percentile_index_permille(sorted.size(), 999)],
        tail_total_ns / static_cast<std::int64_t>(tail_count),
        sorted.back(),
    };
}

void LatencyStats::print_summary(std::ostream& out, const std::string_view name) const {
    const LatencySummary summary = summarize();
    out
        << "latency[" << name << "] "
        << "count=" << summary.count
        << " min_ns=" << summary.min_ns
        << " mean_ns=" << summary.mean_ns
        << " p50_ns=" << summary.p50_ns
        << " p99_ns=" << summary.p99_ns
        << " p99_9_ns=" << summary.p99_9_ns
        << " tail_mean_99_ns=" << summary.tail_mean_99_ns
        << " max_ns=" << summary.max_ns
        << '\n';
}

std::size_t LatencyStats::percentile_index_permille(const std::size_t n, const std::size_t permille) noexcept {
    if (n == 0) {
        return 0;
    }

    return ((n - 1) * permille) / 1000;
}

std::size_t LatencyStats::tail_count_for_top_percent(const std::size_t n, const std::size_t percent) noexcept {
    if (n == 0) {
        return 0;
    }

    const std::size_t numerator = n * percent + 99;
    const std::size_t tail_count = numerator / 100;
    return tail_count == 0 ? 1 : tail_count;
}

}  // namespace llt
