#pragma once

#include "types.h"

namespace llt {

class Strategy {
public:
    Strategy(Price buy_below, Price sell_above, std::int32_t order_qty)
        : buy_below_(buy_below), sell_above_(sell_above), order_qty_(order_qty) {}

    [[nodiscard]] StrategyDecision on_tick(const MarketTick& tick) const noexcept;

private:
    Price buy_below_;
    Price sell_above_;
    std::int32_t order_qty_;
};

}  // namespace llt
