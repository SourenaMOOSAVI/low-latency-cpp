#pragma once
#include "types.h"
#include <vector>
#include <cstdint>
#include <stdexcept>

/**
 * @brief A fixed-size memory pool for MarketData objects.
 * 
 * Provides fast, cache-aligned allocations for low-latency systems by pre-allocating
 * a pool of MarketData objects. Uses a free list to manage deallocated objects, ensuring
 * O(1) allocation and deallocation. Designed to minimize heap fragmentation and runtime
 * overhead in high-performance applications.
 */
class MemoryPool {
public:
    /**
     * @brief Constructs a MemoryPool with a fixed number of MarketData objects.
     * @param size Number of MarketData objects to pre-allocate.
     * @throws std::bad_alloc if memory allocation fails.
     * 
     * Pre-allocates aligned memory for cache efficiency and initializes a free list
     * for O(1) allocations.
     */
    MemoryPool(size_t size) : size_(size) {
        // Allocate raw memory for MarketData objects with alignment
        storage_ = new (std::align_val_t{alignof(MarketData)}) uint8_t[size * sizeof(MarketData)];
        if (!storage_) throw std::bad_alloc();

        // Initialize free list with all slots
        free_list_.reserve(size);
        for (size_t i = 0; i < size; ++i) {
            free_list_.push_back(reinterpret_cast<MarketData*>(storage_ + i * sizeof(MarketData)));
        }
    }

    /**
     * @brief Destructs the MemoryPool, freeing allocated memory.
     * 
     * Ensures all allocated memory is properly deallocated to prevent leaks.
     */
    ~MemoryPool() {
        if (storage_) {
            delete[] storage_;
        }
    }

    /**
     * @brief Allocates a MarketData object from the pool.
     * @return Pointer to a MarketData object, or nullptr if the pool is exhausted.
     * 
     * Provides O(1) allocation by popping from the free list, avoiding heap overhead.
     */
    MarketData* allocate() {
        if (free_list_.empty()) {
            return nullptr;
        }
        MarketData* ptr = free_list_.back();
        free_list_.pop_back();
        return ptr;
    }

    /**
     * @brief Deallocates a MarketData object, returning it to the pool.
     * @param ptr Pointer to the MarketData object to deallocate.
     * 
     * Pushes the object back to the free list for reuse, maintaining O(1) performance.
     */
    void deallocate(MarketData* ptr) {
        if (ptr) {
            free_list_.push_back(ptr);
        }
    }

private:
    uint8_t* storage_ = nullptr;           ///< Raw memory for MarketData objects.
    std::vector<MarketData*> free_list_;   ///< Free list of available MarketData pointers.
    const size_t size_;                    ///< Total number of objects in the pool.
};