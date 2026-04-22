#include <cstdlib>
#include <iostream>

#include "fill_logger.h"
#include "types.h"

namespace {

bool check_eq(const char* label, const std::size_t actual, const std::size_t expected) {
    if (actual == expected) {
        return true;
    }

    std::cerr << label << " expected=" << expected << " actual=" << actual << '\n';
    return false;
}

bool test_hot_struct_sizes() {
    return check_eq("sizeof.Fill", sizeof(llt::Fill), 24) &&
        check_eq("sizeof.TradeEvent", sizeof(llt::TradeEvent), 24) &&
        check_eq("sizeof.PositionState", sizeof(llt::PositionState), 16) &&
        check_eq("sizeof.PnlState", sizeof(llt::PnlState), 24);
}

}  // namespace

int main() {
    if (!test_hot_struct_sizes()) {
        return EXIT_FAILURE;
    }

    std::cout << "layout_tests: all tests passed\n";
    return EXIT_SUCCESS;
}
