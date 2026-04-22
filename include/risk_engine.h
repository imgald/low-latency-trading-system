#pragma once

#include <cstdint>

#include "types.h"

namespace llt {

class RiskEngine {
public:
    explicit RiskEngine(std::int64_t max_abs_position) : max_abs_position_(max_abs_position) {}

    [[nodiscard]] OrderDecision evaluate(
        const PositionState& position,
        const OrderRequest& request) const noexcept;

private:
    std::int64_t max_abs_position_;
};

}  // namespace llt
