#pragma once

#include <cstdint>

#include "types.h"

namespace llt {

struct ExecutionReport {
    bool accepted;
    bool has_fill;
    bool was_partial;
    bool cancel_acked;
    bool stale_quote;
    bool adverse_selection;
    Fill fill;
    std::int32_t leaves_qty;
};

struct ExecutionStats {
    std::int64_t aggressive_fill_count;
    std::int64_t passive_fill_count;
    std::int64_t partial_fill_count;
    std::int64_t cancel_request_count;
    std::int64_t cancel_ack_count;
    std::int64_t stale_fill_count;
    std::int64_t adverse_fill_count;
};

class ExecutionSimulator {
public:
    ExecutionSimulator(
        TimestampNs cancel_latency_ns = 1000,
        TimestampNs stale_after_ns = 1000,
        Price adverse_move_threshold = PRICE_SCALE)
        : cancel_latency_ns_(cancel_latency_ns),
          stale_after_ns_(stale_after_ns),
          adverse_move_threshold_(adverse_move_threshold) {}

    [[nodiscard]] ExecutionReport submit_order(
        const MarketTick& tick,
        const OrderRequest& request,
        ExecutionStyle style) noexcept;

    [[nodiscard]] ExecutionReport on_tick(const MarketTick& tick) noexcept;
    [[nodiscard]] bool request_cancel(TimestampNs ts_ns) noexcept;
    [[nodiscard]] bool has_resting_order() const noexcept { return resting_order_.active; }
    [[nodiscard]] const ExecutionStats& stats() const noexcept { return stats_; }

private:
    struct RestingOrder {
        bool active;
        Side side;
        Price price;
        std::int32_t leaves_qty;
        std::int32_t queue_ahead_qty;
        TimestampNs submit_ts_ns;
        bool cancel_pending;
        TimestampNs cancel_effective_ts_ns;
    };

    [[nodiscard]] ExecutionReport aggressive_fill(
        const MarketTick& tick,
        const OrderRequest& request) noexcept;

    [[nodiscard]] ExecutionReport passive_fill_from_market(
        const MarketTick& tick) noexcept;

    [[nodiscard]] bool is_marketable(const MarketTick& tick) const noexcept;
    [[nodiscard]] std::int32_t visible_opposite_qty(const MarketTick& tick) const noexcept;
    [[nodiscard]] bool detect_adverse_selection(const MarketTick& tick) const noexcept;

    TimestampNs cancel_latency_ns_;
    TimestampNs stale_after_ns_;
    Price adverse_move_threshold_;
    RestingOrder resting_order_{false, Side::Buy, 0, 0, 0, 0, false, 0};
    ExecutionStats stats_{0, 0, 0, 0, 0, 0, 0};
};

}  // namespace llt
