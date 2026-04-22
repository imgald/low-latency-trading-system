#pragma once

#include <cstddef>
#include <cstdint>
#include <iosfwd>
#include <string_view>
#include <vector>

namespace llt {

struct LatencySummary {
    std::size_t count;
    std::int64_t min_ns;
    std::int64_t mean_ns;
    std::int64_t p50_ns;
    std::int64_t p99_ns;
    std::int64_t p99_9_ns;
    // tail_mean_99 is the average of the slowest 1% of samples, which is useful when we want
    // a latency analogue of CVaR/expected shortfall rather than just a single percentile cutoff.
    std::int64_t tail_mean_99_ns;
    std::int64_t max_ns;
};

class LatencyStats {
public:
    void reserve(std::size_t capacity);
    void record(std::int64_t latency_ns);
    [[nodiscard]] bool empty() const noexcept { return samples_.empty(); }
    [[nodiscard]] LatencySummary summarize() const;
    void print_summary(std::ostream& out, std::string_view name) const;

private:
    [[nodiscard]] static std::size_t percentile_index_permille(std::size_t n, std::size_t permille) noexcept;
    [[nodiscard]] static std::size_t tail_count_for_top_percent(std::size_t n, std::size_t percent) noexcept;

    std::vector<std::int64_t> samples_;
};

}  // namespace llt
