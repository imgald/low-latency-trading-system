#include <cstdlib>
#include <iostream>

#include "mock_venue_gateway.h"

namespace {

bool check_true(const char* label, const bool value) {
    if (value) {
        return true;
    }
    std::cerr << label << " expected=true actual=false\n";
    return false;
}

bool check_eq(const char* label, const std::int64_t actual, const std::int64_t expected) {
    if (actual == expected) {
        return true;
    }
    std::cerr << label << " expected=" << expected << " actual=" << actual << '\n';
    return false;
}

bool test_new_ack_and_fill_flow() {
    llt::MockVenueGateway gateway(200, 200);
    const bool sent = gateway.send_new(
        llt::GatewayNewOrder{1, llt::Side::Buy, 50, 100000, llt::ExecutionStyle::Passive, 1000});
    if (!check_true("send_new", sent)) {
        return false;
    }

    const llt::MarketTick tick1{1100, 1100, 100000, 100100, 1, 10, 10};
    const llt::VenueEventBatch batch1 = gateway.on_tick(tick1);
    if (!check_eq("batch1.count", batch1.count, 0)) {
        return false;
    }

    const llt::MarketTick tick2{1200, 1200, 100000, 100000, 1, 10, 20};
    const llt::VenueEventBatch batch2 = gateway.on_tick(tick2);
    if (!(check_eq("batch2.count", batch2.count, 2) &&
        check_true("batch2.new_ack", batch2.events[0].type == llt::VenueEventType::NewAck))) {
        return false;
    }
    if (!check_true("batch2.fill", batch2.events[1].type == llt::VenueEventType::Fill)) {
        return false;
    }

    const llt::MarketTick tick3{1300, 1300, 100000, 100000, 1, 10, 30};
    const llt::VenueEventBatch batch3 = gateway.on_tick(tick3);
    return check_eq("batch3.count", batch3.count, 1) &&
        check_true("batch3.fill", batch3.events[0].type == llt::VenueEventType::Fill);
}

bool test_cancel_ack_flow() {
    llt::MockVenueGateway gateway(200, 200);
    if (!check_true("cancel.send_new", gateway.send_new(
        llt::GatewayNewOrder{2, llt::Side::Buy, 40, 100000, llt::ExecutionStyle::Passive, 1000}))) {
        return false;
    }

    (void) gateway.on_tick(llt::MarketTick{1200, 1200, 100000, 100200, 1, 20, 20});
    if (!check_true("cancel.request", gateway.send_cancel(1300))) {
        return false;
    }

    const llt::VenueEventBatch before_ack =
        gateway.on_tick(llt::MarketTick{1400, 1400, 100000, 100200, 1, 20, 20});
    if (!check_eq("cancel.before_ack_count", before_ack.count, 0)) {
        return false;
    }

    const llt::VenueEventBatch after_ack =
        gateway.on_tick(llt::MarketTick{1500, 1500, 100000, 100200, 1, 20, 20});
    if (!check_eq("cancel.after_ack_count", after_ack.count, 0)) {
        return false;
    }

    const llt::VenueEventBatch final_ack =
        gateway.on_tick(llt::MarketTick{2500, 2500, 100000, 100200, 1, 20, 20});
    return check_eq("cancel.final_ack_count", final_ack.count, 1) &&
        check_true("cancel.ack_type", final_ack.events[0].type == llt::VenueEventType::CancelAck);
}

}  // namespace

int main() {
    const bool ok = test_new_ack_and_fill_flow() && test_cancel_ack_flow();
    if (!ok) {
        return EXIT_FAILURE;
    }

    std::cout << "mock_venue_gateway_tests: all tests passed\n";
    return EXIT_SUCCESS;
}
