#pragma once

#include "types.h"

namespace llt {

class AccountingEngine {
public:
    AccountingEngine() = default;

    void apply_fill(const Fill& fill);
    void mark_to_market(Price mid_price);

    [[nodiscard]] const PositionState& position() const noexcept { return position_state_; }
    [[nodiscard]] const PnlState& pnl() const noexcept { return pnl_state_; }

private:
    PositionState position_state_{0, 0};
    PnlState pnl_state_{0, 0, 0};
};

}  // namespace llt
