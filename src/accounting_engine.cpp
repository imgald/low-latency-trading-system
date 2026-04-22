#include "accounting_engine.h"

#include <cstdlib>

namespace llt {

namespace {

std::int64_t abs_qty(const std::int64_t qty) {
    return qty >= 0 ? qty : -qty;
}

}  // namespace

void AccountingEngine::apply_fill(const Fill& fill) {
    const std::int64_t signed_fill_qty = side_sign(fill.side) * static_cast<std::int64_t>(fill.quantity);
    const std::int64_t current_qty = position_state_.net_qty;

    // Cash is updated for every fill independently of whether the trade opens, reduces, closes,
    // or flips the position. The remaining branches only decide how inventory and PnL evolve.
    pnl_state_.cash -= signed_fill_qty * fill.price;

    if (current_qty == 0 || ((current_qty > 0) == (signed_fill_qty > 0))) {
        const std::int64_t new_qty = current_qty + signed_fill_qty;
        const std::int64_t current_abs = abs_qty(current_qty);
        const std::int64_t fill_abs = abs_qty(signed_fill_qty);
        const std::int64_t new_abs = abs_qty(new_qty);

        if (new_abs == 0) {
            position_state_.net_qty = 0;
            position_state_.avg_price = 0;
            pnl_state_.unrealized_pnl = 0;
            return;
        }

        const std::int64_t weighted_notional =
            static_cast<std::int64_t>(position_state_.avg_price) * current_abs +
            static_cast<std::int64_t>(fill.price) * fill_abs;

        position_state_.net_qty = new_qty;
        position_state_.avg_price = weighted_notional / new_abs;
        return;
    }

    // Opposite-direction fills first close existing inventory. If quantity remains after crossing
    // zero, the residual becomes a new position opened at the incoming fill price.
    const std::int64_t current_direction = current_qty > 0 ? 1 : -1;
    const std::int64_t closing_qty = abs_qty(current_qty) < abs_qty(signed_fill_qty)
        ? abs_qty(current_qty)
        : abs_qty(signed_fill_qty);

    pnl_state_.realized_pnl +=
        (fill.price - position_state_.avg_price) * closing_qty * current_direction;

    const std::int64_t residual_qty = current_qty + signed_fill_qty;
    position_state_.net_qty = residual_qty;

    if (residual_qty == 0) {
        position_state_.avg_price = 0;
        pnl_state_.unrealized_pnl = 0;
        return;
    }

    if ((residual_qty > 0) == (current_qty > 0)) {
        return;
    }

    position_state_.avg_price = fill.price;
}

void AccountingEngine::mark_to_market(const Price mid_price) {
    if (position_state_.net_qty == 0) {
        pnl_state_.unrealized_pnl = 0;
        return;
    }

    const std::int64_t direction = position_state_.net_qty > 0 ? 1 : -1;
    pnl_state_.unrealized_pnl =
        (mid_price - position_state_.avg_price) * abs_qty(position_state_.net_qty) * direction;
}

}  // namespace llt
