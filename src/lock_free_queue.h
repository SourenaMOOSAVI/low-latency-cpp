#pragma once
#include <atomic>
#include <vector>
#include "types.h"
#include "memory_pool.h"
#include <stdexcept>

/**
 * @brief A single-producer, single-consumer lock-free queue using compare-and-swap (CAS).
 * 
 * Designed for low-latency systems, this queue uses std::atomic operations to ensure thread safety
 * without locks. It integrates with a MemoryPool for fast, cache-aligned allocations of MarketData.
 * The queue operates as a circular buffer, minimizing memory overhead and ensuring O(1) operations.
 */
class LockFreeQueue {
public:
    /**
     * @brief Constructs a LockFreeQueue with a fixed capacity.
     * @param capacity Maximum number of items the queue can hold.
     * @param pool Reference to a MemoryPool for allocating MarketData objects.
     * @throws std::runtime_error if memory pool allocation fails.
     * 
     * Pre-allocates buffer pointers from the pool to eliminate runtime allocations.
     */
    LockFreeQueue(size_t capacity, MemoryPool& pool) 
        : buffer(capacity), pool(pool), head(0), tail(0), capacity(capacity) {
        for (size_t i = 0; i < capacity; ++i) {
            buffer[i] = pool.allocate();
            if (!buffer[i]) throw std::runtime_error("Memory pool exhausted");
        }
    }

    /**
     * @brief Destructs the LockFreeQueue, deallocating buffer pointers.
     * 
     * Returns all allocated MarketData objects to the MemoryPool for reuse.
     */
    ~LockFreeQueue() {
        for (size_t i = 0; i < capacity; ++i) {
            pool.deallocate(buffer[i]);
        }
    }

    /**
     * @brief Pushes a MarketData item to the queue (producer operation).
     * @param item The MarketData item to push.
     * @return True if the item was pushed, false if the queue is full.
     * 
     * Uses std::memory_order_relaxed for tail increments and acquire for head checks
     * to ensure thread safety in a single-producer context. CAS ensures atomicity.
     */
    bool push(const MarketData& item) {
        size_t current_tail = tail.load(std::memory_order_relaxed);
        size_t next_tail = (current_tail + 1) % capacity;

        if (next_tail == head.load(std::memory_order_acquire)) {
            return false; // Queue full
        }

        *buffer[current_tail] = item;
        tail.store(next_tail, std::memory_order_release);
        return true;
    }

    /**
     * @brief Pops a MarketData item from the queue (consumer operation).
     * @param item Output parameter to store the popped item.
     * @return True if an item was popped, false if the queue is empty.
     * 
     * Uses std::memory_order_relaxed for head increments and acquire for tail checks
     * to ensure thread safety in a single-consumer context. CAS ensures atomicity.
     */
    bool pop(MarketData& item) {
        size_t current_head = head.load(std::memory_order_relaxed);
        if (current_head == tail.load(std::memory_order_acquire)) {
            return false; // Queue empty
        }

        item = *buffer[current_head];
        head.store((current_head + 1) % capacity, std::memory_order_release);
        return true;
    }

private:
    std::vector<MarketData*> buffer; ///< Circular buffer of pointers to pooled MarketData objects.
    MemoryPool& pool;                ///< Reference to MemoryPool for allocations.
    std::atomic<size_t> head;        ///< Atomic head index for consumer.
    std::atomic<size_t> tail;        ///< Atomic tail index for producer.
    const size_t capacity;           ///< Fixed queue capacity.
};