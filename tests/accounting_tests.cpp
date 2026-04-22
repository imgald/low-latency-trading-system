#include <cstdlib>
#include <iostream>

#include "accounting_engine.h"

namespace {

using llt::AccountingEngine;
using llt::Fill;
using llt::Price;
using llt::Side;

constexpr Price px(const std::int64_t whole) {
    return whole * llt::PRICE_SCALE;
}

bool check_eq(const char* label, const std::int64_t actual, const std::int64_t expected) {
    if (actual == expected) {
        return true;
    }

    std::cerr << label << " expected=" << expected << " actual=" << actual << '\n';
    return false;
}

bool test_basic_open() {
    AccountingEngine engine;
    engine.apply_fill(Fill{1, px(10), 100, Side::Buy});

    return check_eq("basic_open.net_qty", engine.position().net_qty, 100) &&
        check_eq("basic_open.avg_price", engine.position().avg_price, px(10)) &&
        check_eq("basic_open.realized", engine.pnl().realized_pnl, 0);
}

bool test_same_side_add() {
    AccountingEngine engine;
    engine.apply_fill(Fill{1, px(10), 100, Side::Buy});
    engine.apply_fill(Fill{2, px(12), 100, Side::Buy});

    return check_eq("same_side_add.net_qty", engine.position().net_qty, 200) &&
        check_eq("same_side_add.avg_price", engine.position().avg_price, px(11));
}

bool test_partial_close() {
    AccountingEngine engine;
    engine.apply_fill(Fill{1, px(10), 100, Side::Buy});
    engine.apply_fill(Fill{2, px(15), 50, Side::Sell});

    return check_eq("partial_close.net_qty", engine.position().net_qty, 50) &&
        check_eq("partial_close.avg_price", engine.position().avg_price, px(10)) &&
        check_eq("partial_close.realized", engine.pnl().realized_pnl, px(5) * 50);
}

bool test_full_close() {
    AccountingEngine engine;
    engine.apply_fill(Fill{1, px(10), 100, Side::Buy});
    engine.apply_fill(Fill{2, px(8), 100, Side::Sell});

    return check_eq("full_close.net_qty", engine.position().net_qty, 0) &&
        check_eq("full_close.avg_price", engine.position().avg_price, 0) &&
        check_eq("full_close.realized", engine.pnl().realized_pnl, -px(2) * 100);
}

bool test_flip_position() {
    AccountingEngine engine;
    engine.apply_fill(Fill{1, px(10), 100, Side::Buy});
    engine.apply_fill(Fill{2, px(12), 150, Side::Sell});

    return check_eq("flip_position.net_qty", engine.position().net_qty, -50) &&
        check_eq("flip_position.avg_price", engine.position().avg_price, px(12)) &&
        check_eq("flip_position.realized", engine.pnl().realized_pnl, px(2) * 100);
}

bool test_short_cover_partial() {
    AccountingEngine engine;
    engine.apply_fill(Fill{1, px(10), 100, Side::Sell});
    engine.apply_fill(Fill{2, px(8), 50, Side::Buy});

    return check_eq("short_cover_partial.net_qty", engine.position().net_qty, -50) &&
        check_eq("short_cover_partial.avg_price", engine.position().avg_price, px(10)) &&
        check_eq("short_cover_partial.realized", engine.pnl().realized_pnl, px(2) * 50);
}

}  // namespace

int main() {
    const bool ok =
        test_basic_open() &&
        test_same_side_add() &&
        test_partial_close() &&
        test_full_close() &&
        test_flip_position() &&
        test_short_cover_partial();

    if (!ok) {
        return EXIT_FAILURE;
    }

    std::cout << "accounting_tests: all tests passed\n";
    return EXIT_SUCCESS;
}
