#include "lock_free_queue.h"
#include "types.h"
#include "memory_pool.h"
#include <queue>
#include <mutex>
#include <thread>
#include <chrono>
#include <iostream>

struct Benchmark {
    static void run_mutex_queue(size_t iterations) {
        std::queue<MarketData> queue;
        std::mutex mutex;
        bool running = true;
        size_t processed = 0;

        auto producer = [&]() {
            for (size_t i = 0; i < iterations; ++i) {
                MarketData data{"TEST", 100.0, 100};
                std::lock_guard<std::mutex> lock(mutex);
                queue.push(data);
            }
        };

        auto consumer = [&]() {
            while (processed < iterations && running) {
                MarketData data;
                bool hasData = false;
                {
                    std::lock_guard<std::mutex> lock(mutex);
                    if (!queue.empty()) {
                        data = queue.front();
                        queue.pop();
                        hasData = true;
                    }
                }
                if (hasData) {
                    ++processed;
                }
            }
        };

        auto start = std::chrono::high_resolution_clock::now();
        std::thread prod(producer);
        std::thread cons(consumer);
        prod.join();
        running = false;
        cons.join();
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
        std::cout << "Mutex Queue: " << iterations << " items, "
                  << duration / 1000.0 << " ms, "
                  << (iterations * 1000000.0 / duration) << " items/sec\n";
    }

    static void run_lock_free_queue(size_t iterations) {
        MemoryPool pool(1000);
        LockFreeQueue queue(1000, pool);
        bool running = true;
        size_t processed = 0;

        auto producer = [&]() {
            for (size_t i = 0; i < iterations; ++i) {
                MarketData data{"TEST", 100.0, 100};
                while (!queue.push(data)) {
                    std::this_thread::yield();
                }
            }
        };

        auto consumer = [&]() {
            MarketData data;
            while (processed < iterations && running) {
                if (queue.pop(data)) {
                    ++processed;
                } else {
                    std::this_thread::yield();
                }
            }
        };

        auto start = std::chrono::high_resolution_clock::now();
        std::thread prod(producer);
        std::thread cons(consumer);
        prod.join();
        running = false;
        cons.join();
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
        std::cout << "Lock-Free Queue (Memory Pool): " << iterations << " items, "
                  << duration / 1000.0 << " ms, "
                  << (iterations * 1000000.0 / duration) << " items/sec\n";
    }

    static void run_allocation_benchmark(size_t iterations) {
        // Standard allocation
        auto start = std::chrono::high_resolution_clock::now();
        for (size_t i = 0; i < iterations; ++i) {
            MarketData* data = new MarketData{"TEST", 100.0, 100};
            delete data;
        }
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
        std::cout << "Standard Allocation: " << iterations << " items, "
                  << duration / 1000.0 << " ms, "
                  << (iterations * 1000000.0 / duration) << " allocs/sec\n";

        // Memory pool allocation
        MemoryPool pool(iterations);
        start = std::chrono::high_resolution_clock::now();
        for (size_t i = 0; i < iterations; ++i) {
            MarketData* data = pool.allocate();
            pool.deallocate(data);
        }
        end = std::chrono::high_resolution_clock::now();
        duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
        std::cout << "Memory Pool Allocation: " << iterations << " items, "
                  << duration / 1000.0 << " ms, "
                  << (iterations * 1000000.0 / duration) << " allocs/sec\n";
    }
};

int main() {
    const size_t iterations = 1'000'000;
    std::cout << "sizeof(MarketData): " << sizeof(MarketData) << " bytes\n";
    Benchmark::run_mutex_queue(iterations);
    Benchmark::run_lock_free_queue(iterations);
    Benchmark::run_allocation_benchmark(iterations);
    return 0;
}