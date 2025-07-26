# Concurrency in High-Frequency Trading Systems

## Overview
Concurrency is critical in high-frequency trading (HFT) systems to process market data and execute trades with minimal latency. This project demonstrates modern C++ concurrency techniques, evolving from simple mutex-based queues to advanced lock-free data structures, NUMA-aware memory management, and thread affinity.

## Threads and Task Partitioning
Threads (`std::thread`) enable concurrent execution of tasks. In this project:
- A **producer thread** generates or reads market data and pushes it to a shared queue.
- A **consumer thread** processes data from the queue (e.g., parsing, logging, or acting on market data).
- Threads are managed by the `MarketDataParser` class, which starts and stops them cleanly.

**Code Example**:
```cpp
producerThread = std::thread(&MarketDataParser::generateData, this);
consumerThread = std::thread(&MarketDataParser::processData, this);
```

## Lock-Free Queues
To minimize latency and contention, the project uses a single-producer, single-consumer **lock-free queue** (`LockFreeQueue`). This eliminates the need for mutexes, allowing threads to communicate efficiently using atomic operations.

- The producer thread pushes data to the lock-free queue.
- The consumer thread pops data from the queue.
- The queue is designed for high throughput and minimal synchronization overhead.

**Design Note:** Earlier versions used a mutex-protected `std::queue`, but this was replaced for better scalability.

## Memory Pools and NUMA Awareness
Memory allocation can be a bottleneck in low-latency systems. This project uses a **NUMA-aware memory pool** (`MemoryPool`) for fast, cache-friendly allocation of `MarketData` objects.

- Reduces allocation overhead and improves cache locality.
- Optimized for multi-core, multi-socket systems.

## Thread Affinity
To further reduce latency, threads are pinned to specific CPU cores using **thread affinity** utilities. This ensures:
- Better cache and NUMA locality.
- Reduced context switching and memory access latency.

## Race Conditions and Synchronization
A race condition occurs when multiple threads access shared data concurrently without proper synchronization, leading to unpredictable behavior. The lock-free queue uses atomic operations to prevent race conditions without the overhead of mutexes.

## Benchmarking and Performance
The project includes a benchmarking suite to compare:
- Mutex-based queue vs. lock-free queue.
- Standard allocation vs. memory pool allocation.

This helps quantify the performance benefits of advanced concurrency techniques.

## Design Considerations
- **Scalability:** Lock-free and NUMA-aware designs scale better under high contention.
- **Maintainability:** The `MarketDataParser` class encapsulates thread and resource management, making the code modular and extensible.
- **Modern C++:** The codebase uses C++20 features, RAII, and smart pointers for safety and clarity.

## Next Steps
- Explore multi-producer/multi-consumer lock-free queues.
- Add more advanced synchronization primitives (e.g., condition variables).
- Expand benchmarking and profiling for real-world scenarios.