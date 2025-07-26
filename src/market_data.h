#pragma once
#include "lock_free_queue.h"
#include "memory_pool.h"
#include "types.h"
#include <atomic>
#include <thread>

/**
 * @brief Parses and processes MarketData using a lock-free queue and memory pool.
 * Manages producer and consumer threads for low-latency data handling.
 */
class MarketDataParser {
public:
    MarketDataParser();
    ~MarketDataParser();
    void start();
    void stop();
    bool processNext(MarketData& data);

private:
    void generateData();
    void processData();

    std::atomic<bool> running;
    MemoryPool pool;
    LockFreeQueue dataQueue;
    std::thread producerThread;
    std::thread consumerThread;
    size_t packet_count;
};