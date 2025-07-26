# Concurrency in High-Frequency Trading Systems

## Overview
Concurrency is critical in high-frequency trading (HFT) systems to process market data and execute trades with minimal latency. This document explains the concurrency concepts used in Week 1 of the HFT system project.

## Threads
Threads (`std::thread`) enable concurrent execution of tasks. In this project:
- A **reader thread** reads mock market data from a file and pushes it to a shared queue.
- A **processor thread** consumes data from the queue and processes it (e.g., prints it).
- Threads are managed by the `MarketDataParser` class, which starts and stops them cleanly.

**Code Example**:
```cpp
readerThread = std::thread(&MarketDataParser::readData, this);
processorThread = std::thread(&MarketDataParser::processData, this);
```

## Mutexes
A mutex (`std::mutex`) ensures exclusive access to shared resources, preventing data corruption. We use a mutex to protect the shared `std::queue<MarketData>`:
- The reader thread locks the mutex when pushing data to the queue.
- The processor thread locks the mutex when popping data.
- `std::lock_guard` is used for RAII-style locking, ensuring the mutex is released even if an exception occurs.

**Code Example**:
```cpp
{
    std::lock_guard<std::mutex> lock(queueMutex);
    dataQueue.push(data);
}
```

## Race Conditions
A race condition occurs when multiple threads access shared data concurrently without proper synchronization, leading to unpredictable behavior. For example:
- Without a mutex, the reader could push data while the processor pops, corrupting the queue.
- The mutex ensures only one thread accesses the queue at a time, eliminating race conditions.

## Design Considerations
- **Scalability**: The mutex-based queue is simple but may become a bottleneck under high contention. Future weeks will explore lock-free alternatives.
- **Maintainability**: The `MarketDataParser` class encapsulates thread management, making the code modular and easier to extend.

## Next Steps
In Week 2, we'll replace the mutex-based queue with a lock-free queue using `std::atomic` to improve performance and explore condition variables for more efficient synchronization.