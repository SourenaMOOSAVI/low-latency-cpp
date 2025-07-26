# Low-Latency System Design with Modern C++

A C++20 project demonstrating low-latency and high-performance system design techniques using a simplified high-frequency trading (HFT) system as a learning example.

## Overview
This project focuses on modern C++ techniques for low-latency systems, covering:
- **Concurrency**: Lock-free data structures with compare-and-swap (CAS).
- **Low-Latency Design**: Cache-aligned structures and NUMA-aware thread scheduling.
- **Memory Management**: Custom memory pools for fast allocations.

The codebase simulates a producer-consumer model processing market data, serving as a practical platform to explore performance optimization.

## Techniques Demonstrated
- **LockFreeQueue**: Single-producer, single-consumer queue using `std::atomic` (~5.58M items/sec).
- **MemoryPool**: Custom allocator for `MarketData` objects (~25.04M allocs/sec).
- **Thread Affinity**: CPU pinning to reduce context switches and NUMA effects.
- **Cache Optimization**: `MarketData` aligned to 64-byte cache lines.
- **Adaptive Polling**: Tuned busy-waiting with dynamic sleeps to eliminate excessive logging and improve efficiency.
- **Thread-Safe Logging**: Singleton logger with mutex for reliable file and console output.

## Building
```bash
mkdir build && cd build
cmake ..
make
```

**Dependencies**:
- C++20 compiler (e.g., GCC 13.3)
- `libnuma-dev` for thread affinity

## Running
```bash
./build/hft_system
```
- Processes 30 simulated market data items (10 batches × 3).
- Outputs logs to `hft_system.log`.
- Press Enter to stop (~1.2s).

## Benchmarking
```bash
./build/benchmark
```
- **Results**:
  - `LockFreeQueue`: ~5.58M items/sec
  - `MemoryPool`: ~25.04M allocs/sec
  - `Mutex Queue`: ~2.11M items/sec (baseline)
  - `Standard Allocation`: ~14.49M allocs/sec (baseline)

## Project Structure
- `src/main.cpp`: Program entry point.
- `src/market_data.cpp`: Producer-consumer with optimized polling and logging.
- `src/types.h`: Cache-aligned `MarketData` structure.
- `src/memory_pool.h`: Custom memory pool.
- `src/lock_free_queue.h`: Lock-free queue implementation.
- `src/thread_affinity.h`, `src/thread_affinity.cpp`: Thread affinity utilities.
- `src/logger.h`: Thread-safe logging singleton.

## Week 4 Achievements
- Processed 30 market data items across 10 batches, verified by producer and consumer logs.
- Implemented thread-safe logging, fixing missing consumer logs.
- Optimized polling to eliminate excessive `Queue empty, yielding` logs.
- Achieved high-performance benchmarks, exceeding Phase 1 goals.
- Added Doxygen comments for maintainability.
- Prepared GitHub-ready codebase with README and `.gitignore`.

## Known Improvements
- **Duplicate Logs**: `Stopping consumer thread` and `All threads stopped` appear twice due to redundant `stop()` calls in `main.cpp` and destructor.
- **Throughput**: Simulation achieves ~13.95 items/sec (with 100ms batch delays) vs. target ~30 items/sec.
- **Benchmark Variability**: Slight regression in `LockFreeQueue` (~5.58M vs. ~5.98M) and `MemoryPool` (~25.04M vs. ~30.79M), likely due to system load.

## Future Work
- **Phase 2 (Weeks 5–6)**:
  - Prevent redundant `stop()` calls with a `stopped` flag in `MarketDataParser`.
  - Improve throughput to ~30 items/sec using `std::condition_variable` for efficient synchronization.
  - Implement batch processing in `LockFreeQueue` to reduce overhead.
  - Use NUMA-aware memory allocators with `libnuma` for better performance.
  - Run benchmarks in a controlled environment to minimize variability.
- **Additional Techniques**:
  - Explore low-latency networking (e.g., UDP with zero-copy).
  - Implement cache-aware data structures (e.g., slab allocators).
  - Extend to multi-producer, multi-consumer queues for scalability.

## License
MIT