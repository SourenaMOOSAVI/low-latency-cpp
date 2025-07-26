#include "market_data.h"
#include "thread_affinity.h"
#include "logger.h"
#include <chrono>
#include <iostream>
#include <sstream>
#include <thread>
#include <vector>

/**
 * @brief Constructs a MarketDataParser with a memory pool and lock-free queue.
 * Initializes packet_count to 0 for tracking processed data batches.
 * Uses MemoryPool to pre-allocate MarketData objects, minimizing runtime allocations.
 */
MarketDataParser::MarketDataParser() 
    : running(false), pool(10000), dataQueue(10000, pool), packet_count(0) {
    Logger::getInstance().log("MarketDataParser constructed, packet_count: " + std::to_string(packet_count));
    Logger::getInstance().log("MarketDataParser constructed", true);
}

/**
 * @brief Destructor ensures threads are stopped to prevent resource leaks.
 */
MarketDataParser::~MarketDataParser() {
    stop();
}

/**
 * @brief Starts producer and consumer threads for data generation and processing.
 * Resets packet_count to 0. Threads are launched with std::thread for concurrency.
 */
void MarketDataParser::start() {
    running = true;
    packet_count = 0;
    Logger::getInstance().log("Starting producer thread, initial packet_count: " + std::to_string(packet_count));
    Logger::getInstance().log("Starting consumer thread");
    producerThread = std::thread(&MarketDataParser::generateData, this);
    consumerThread = std::thread(&MarketDataParser::processData, this);
    Logger::getInstance().log("Threads launched");
    Logger::getInstance().log("Threads launched\nPress Enter to stop the program...", true);
}

/**
 * @brief Stops threads and joins them, ensuring queue is drained.
 * Uses delays to allow producer to complete and consumer to process remaining items.
 */
void MarketDataParser::stop() {
    std::this_thread::sleep_for(std::chrono::milliseconds(1100));
    running = false;
    Logger::getInstance().log("Stopping consumer thread");
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    if (producerThread.joinable()) {
        Logger::getInstance().log("Joining producer thread");
        producerThread.join();
    }
    if (consumerThread.joinable()) {
        Logger::getInstance().log("Joining consumer thread");
        consumerThread.join();
    }
    Logger::getInstance().log("All threads stopped");
    Logger::getInstance().log("All threads stopped", true);
}

/**
 * @brief Processes the next MarketData item from the lock-free queue.
 * @param data Output parameter for the popped data.
 * @return True if data was popped, false otherwise.
 * Used for external access to queue data, demonstrating queue interface.
 */
bool MarketDataParser::processNext(MarketData& data) {
    return dataQueue.pop(data);
}

/**
 * @brief Generates simulated MarketData and pushes to the lock-free queue.
 * Simulates 10 batches of 3 MarketData items each, with 100ms delays to mimic market data arrival.
 * Uses thread affinity to pin to CPU 0, reducing context switches and NUMA effects.
 */
void MarketDataParser::generateData() {
    Logger& logger = Logger::getInstance();
    try {
        setThreadAffinity(std::this_thread::get_id(), 0);
        logger.log("Producer thread affinity set to CPU 0");
    } catch (const std::exception& e) {
        logger.log("Producer thread failed to set affinity: " + std::string(e.what()));
        logger.log("Producer affinity error", true);
        running = false;
        return;
    }

    logger.log("Producer thread started, packet_count: " + std::to_string(packet_count));
    size_t items_pushed = 0;

    try {
        for (int batch = 0; batch < 10 && running; ++batch) {
            std::vector<MarketData> batch_data = {
                {"AAPL", 150.25 + batch, 1000 + batch},
                {"GOOG", 2750.1 + batch, 500 + batch},
                {"MSFT", 300.75 + batch, 800 + batch}
            };

            logger.log("Generating batch " + std::to_string(batch + 1) + "/10");

            for (const auto& data : batch_data) {
                while (!dataQueue.push(data) && running) {
                    logger.log("Queue full, retrying for: " + std::string(data.symbol));
                    std::this_thread::sleep_for(std::chrono::microseconds(1));
                }
                if (!running) break;
                std::ostringstream oss;
                oss << "Pushed to queue: " << data.symbol << ", " << data.price << ", " << data.volume;
                logger.log(oss.str());
                ++items_pushed;
            }

            ++packet_count;
            logger.log("Processed batch " + std::to_string(packet_count) + ", items pushed: " + std::to_string(items_pushed));
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    } catch (const std::exception& e) {
        logger.log("Producer error: " + std::string(e.what()));
        running = false;
    }

    logger.log("Producer thread exiting, total items pushed: " + std::to_string(items_pushed));
}

/**
 * @brief Consumes MarketData from the lock-free queue and processes it.
 * Uses adaptive polling to balance low-latency and CPU efficiency.
 * Pins to CPU 1 to avoid contention with producer, optimizing NUMA performance.
 */
void MarketDataParser::processData() {
    Logger& logger = Logger::getInstance();
    try {
        setThreadAffinity(std::this_thread::get_id(), 1);
        logger.log("Consumer thread affinity set to CPU 1");
    } catch (const std::exception& e) {
        logger.log("Consumer thread failed to set affinity: " + std::string(e.what()));
        logger.log("Consumer affinity error", true);
        return;
    }

    logger.log("Consumer thread started");
    size_t processed_count = 0;
    auto start = std::chrono::high_resolution_clock::now();
    size_t empty_count = 0;
    size_t yield_count = 0;
    size_t sleep_count = 0;

    try {
        while (running) {
            MarketData data;
            if (dataQueue.pop(data)) {
                std::ostringstream oss;
                oss << "Processed: " << data.symbol << ", Price: " << data.price << ", Volume: " << data.volume;
                logger.log(oss.str());
                ++processed_count;
                empty_count = 0;
                yield_count = 0;
                sleep_count = 0;
            } else {
                ++empty_count;
                if (empty_count < 10000) continue; // Busy-wait for low latency
                if (empty_count < 100000) {
                    std::this_thread::yield(); // Yield for moderate contention
                    continue;
                }
                if (++yield_count % 1000 == 0) {
                    logger.log("Queue empty, yielding");
                }
                // Dynamic sleep: start at 10µs, double up to 100µs
                auto sleep_us = std::min(10ULL << sleep_count, 100ULL);
                std::this_thread::sleep_for(std::chrono::microseconds(sleep_us));
                if (sleep_count < 4) ++sleep_count; // Cap at 80µs
                empty_count = 10000; // Prevent overflow
            }
        }

        // Drain remaining items in queue
        MarketData data;
        while (dataQueue.pop(data)) {
            std::ostringstream oss;
            oss << "Processed: " << data.symbol << ", Price: " << data.price << ", Volume: " << data.volume;
            logger.log(oss.str());
            ++processed_count;
        }

        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
        std::ostringstream oss;
        oss << "Consumer processed " << processed_count << " items in "
            << duration / 1000.0 << " ms, " << (processed_count * 1e6 / duration) << " items/sec";
        logger.log(oss.str());
        logger.log("Consumer thread exiting");
    } catch (const std::exception& e) {
        logger.log("Consumer error: " + std::string(e.what()));
        logger.log("Consumer error", true);
    }
}