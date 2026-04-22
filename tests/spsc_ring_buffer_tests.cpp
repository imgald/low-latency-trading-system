#include <cstdlib>
#include <iostream>

#include "spsc_ring_buffer.h"

namespace {

bool check_true(const char* label, const bool value) {
    if (value) {
        return true;
    }

    std::cerr << label << " expected=true actual=false\n";
    return false;
}

bool check_false(const char* label, const bool value) {
    if (!value) {
        return true;
    }

    std::cerr << label << " expected=false actual=true\n";
    return false;
}

bool check_eq(const char* label, const int actual, const int expected) {
    if (actual == expected) {
        return true;
    }

    std::cerr << label << " expected=" << expected << " actual=" << actual << '\n';
    return false;
}

bool test_fifo_behavior() {
    llt::SpscRingBuffer<int> queue(4);
    int value = 0;

    return check_true("fifo.push1", queue.try_push(1)) &&
        check_true("fifo.push2", queue.try_push(2)) &&
        check_true("fifo.pop1", queue.try_pop(value)) &&
        check_eq("fifo.value1", value, 1) &&
        check_true("fifo.pop2", queue.try_pop(value)) &&
        check_eq("fifo.value2", value, 2) &&
        check_false("fifo.pop_empty", queue.try_pop(value));
}

bool test_full_and_reuse() {
    llt::SpscRingBuffer<int> queue(2);
    int value = 0;

    return check_true("full.push1", queue.try_push(10)) &&
        check_true("full.push2", queue.try_push(20)) &&
        check_false("full.push3", queue.try_push(30)) &&
        check_true("full.pop1", queue.try_pop(value)) &&
        check_eq("full.value1", value, 10) &&
        check_true("full.push_after_pop", queue.try_push(30)) &&
        check_true("full.pop2", queue.try_pop(value)) &&
        check_eq("full.value2", value, 20) &&
        check_true("full.pop3", queue.try_pop(value)) &&
        check_eq("full.value3", value, 30);
}

}  // namespace

int main() {
    const bool ok = test_fifo_behavior() && test_full_and_reuse();
    if (!ok) {
        return EXIT_FAILURE;
    }

    std::cout << "spsc_ring_buffer_tests: all tests passed\n";
    return EXIT_SUCCESS;
}
