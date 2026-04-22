#include "layout_utils.h"

#include <ostream>

#include "fill_logger.h"
#include "types.h"

namespace llt {

void print_layout_summary(std::ostream& out) {
    out << "layout[MarketTick] size=" << sizeof(MarketTick) << " align=" << alignof(MarketTick) << '\n';
    out << "layout[Fill] size=" << sizeof(Fill) << " align=" << alignof(Fill) << '\n';
    out << "layout[TradeEvent] size=" << sizeof(TradeEvent) << " align=" << alignof(TradeEvent) << '\n';
    out << "layout[PositionState] size=" << sizeof(PositionState) << " align=" << alignof(PositionState) << '\n';
    out << "layout[PnlState] size=" << sizeof(PnlState) << " align=" << alignof(PnlState) << '\n';
}

}  // namespace llt
