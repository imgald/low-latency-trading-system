#pragma once

#include <atomic>
#include <cstddef>
#include <vector>

namespace llt {

template <typename T>
class SpscRingBuffer {
public:
    explicit SpscRingBuffer(const std::size_t capacity)
        // Keep one slot empty so head == tail means "empty" and next_head == tail means "full".
        : capacity_(capacity + 1),
          buffer_(capacity_) {}

    [[nodiscard]] bool try_push(const T& value) noexcept {
        const std::size_t head = head_.load(std::memory_order_relaxed);
        const std::size_t next_head = increment(head);
        if (next_head == tail_.load(std::memory_order_acquire)) {
            return false;
        }

        // Publish the payload before moving head so the consumer never observes a slot as ready
        // until its data has been written.
        buffer_[head] = value;
        head_.store(next_head, std::memory_order_release);
        return true;
    }

    [[nodiscard]] bool try_pop(T& value) noexcept {
        const std::size_t tail = tail_.load(std::memory_order_relaxed);
        if (tail == head_.load(std::memory_order_acquire)) {
            return false;
        }

        // Read the payload before advancing tail so the producer cannot reuse the slot early.
        value = buffer_[tail];
        tail_.store(increment(tail), std::memory_order_release);
        return true;
    }

    [[nodiscard]] bool empty() const noexcept {
        return head_.load(std::memory_order_acquire) == tail_.load(std::memory_order_acquire);
    }

    [[nodiscard]] std::size_t capacity() const noexcept {
        return capacity_ - 1;
    }

private:
    [[nodiscard]] std::size_t increment(const std::size_t index) const noexcept {
        return (index + 1) % capacity_;
    }

    const std::size_t capacity_;
    std::vector<T> buffer_;
    // Separate producer and consumer cursors onto different cache lines to reduce false sharing.
    alignas(64) std::atomic<std::size_t> head_{0};
    alignas(64) std::atomic<std::size_t> tail_{0};
};

}  // namespace llt
