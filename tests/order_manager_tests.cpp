#include <cstdlib>
#include <iostream>

#include "order_manager.h"

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

bool check_state(const char* label, const llt::OrderState actual, const llt::OrderState expected) {
    if (actual == expected) {
        return true;
    }
    std::cerr << label << " state mismatch\n";
    return false;
}

bool test_new_ack_partial_fill_cancel() {
    llt::OrderManager om;
    llt::Fill fill{};
    if (!check_true("submit_new", om.submit_new(101, llt::Side::Buy, 100000, 100))) {
        return false;
    }
    if (!check_state("pending_new", om.order().state, llt::OrderState::PendingNewAck)) {
        return false;
    }
    if (!check_true("new_ack", om.on_venue_event(
        llt::VenueEvent{llt::VenueEventType::NewAck, 1000, 555, 0, 0}, fill))) {
        return false;
    }
    if (!check_state("acked", om.order().state, llt::OrderState::Acked)) {
        return false;
    }
    if (!check_true("partial_fill", om.on_venue_event(
        llt::VenueEvent{llt::VenueEventType::Fill, 1100, 555, 40, 99950}, fill))) {
        return false;
    }
    if (!(check_state("partially_filled", om.order().state, llt::OrderState::PartiallyFilled) &&
        check_eq("leaves_after_partial", om.order().leaves_qty, 60) &&
        check_eq("fill_qty", fill.quantity, 40))) {
        return false;
    }
    if (!check_true("request_cancel", om.request_cancel())) {
        return false;
    }
    if (!check_state("pending_cancel", om.order().state, llt::OrderState::PendingCancel)) {
        return false;
    }
    if (!check_true("cancel_ack", om.on_venue_event(
        llt::VenueEvent{llt::VenueEventType::CancelAck, 1200, 555, 0, 0}, fill))) {
        return false;
    }
    return check_state("canceled", om.order().state, llt::OrderState::Canceled);
}

bool test_fill_to_complete() {
    llt::OrderManager om;
    llt::Fill fill{};
    if (!check_true("submit_new_full", om.submit_new(202, llt::Side::Sell, 101000, 30))) {
        return false;
    }
    if (!check_true("new_ack_full", om.on_venue_event(
        llt::VenueEvent{llt::VenueEventType::NewAck, 2000, 777, 0, 0}, fill))) {
        return false;
    }
    if (!check_true("final_fill", om.on_venue_event(
        llt::VenueEvent{llt::VenueEventType::Fill, 2100, 777, 30, 101200}, fill))) {
        return false;
    }
    return check_state("filled", om.order().state, llt::OrderState::Filled) &&
        check_eq("leaves_zero", om.order().leaves_qty, 0);
}

}  // namespace

int main() {
    const bool ok = test_new_ack_partial_fill_cancel() && test_fill_to_complete();
    if (!ok) {
        return EXIT_FAILURE;
    }

    std::cout << "order_manager_tests: all tests passed\n";
    return EXIT_SUCCESS;
}
