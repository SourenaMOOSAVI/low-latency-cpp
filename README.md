# Low-Latency C++ System

## Purpose
This project demonstrates low-latency, high-performance system design in C++ for real-time data processing, such as high-frequency trading (HFT). It focuses on:
- Lock-free data structures
- NUMA-aware memory management
- Thread affinity and concurrency
- Performance benchmarking

## Key Components
- **Market Data Pipeline** (`src/market_data.cpp`, `src/market_data.h`):
  - Producer/consumer threads for market data
  - Lock-free queue and memory pool for minimal latency
  - Thread affinity for NUMA optimization
  - Condition variables for efficient waiting
- **Lock-Free Queue** (`src/lock_free_queue.h`):
  - Single-producer, single-consumer lock-free queue
  - Batch operations for throughput
- **Memory Pool** (`src/memory_pool.h`):
  - NUMA-aware, fast allocation for `MarketData`
- **Thread Affinity** (`src/thread_affinity.cpp`, `src/thread_affinity.h`):
  - Pin threads to CPU cores for cache/NUMA locality
- **Logger** (`src/logger.h`):
  - Thread-safe singleton logger
- **Benchmarking** (`src/benchmark.cpp`):
  - Compares mutex queue vs. lock-free queue, and standard vs. pool allocation
- **Types** (`src/types.h`):
  - Shared data structures (e.g., `MarketData`)

## Project Structure
```
low-latency-cpp/
├── CMakeLists.txt
├── README.md
├── LICENSE
├── .gitignore
├── data/
│   └── mock_market_data.txt
├── docs/
│   └── concurrency.md
├── src/
│   ├── benchmark.cpp
│   ├── lock_free_queue.h
│   ├── logger.h
│   ├── main.cpp
│   ├── market_data.cpp
│   ├── market_data.h
│   ├── memory_pool.h
│   ├── thread_affinity.cpp
│   ├── thread_affinity.h
└── └── types.h
```

## Building
```bash
mkdir build && cd build
cmake ..
make
```
**Dependencies:**
- C++20 compiler (e.g., GCC 13.3)
- `libnuma-dev` for thread affinity

## Running
```bash
./build/hft_system
```
- Processes simulated market data in batches
- Logs output to `hft_system.log`
- Press Enter to stop

## Benchmarking
```bash
./build/benchmark
```
- Compares lock-free queue, memory pool, mutex queue, and standard allocation

## Further Improvements
- **Error Handling & Robustness:**
  - Use custom exceptions and RAII for resource management
  - Ensure all resources are released on error
- **Thread Management:**
  - Use RAII wrappers (e.g., `std::jthread` in C++20)
  - Prefer condition variables or atomics over `sleep_for` for synchronization
- **Lock-Free Queue:**
  - Review atomic memory ordering
  - Benchmark against established libraries (e.g., MoodyCamel’s ConcurrentQueue)
- **Memory Pool:**
  - Profile NUMA benefits; consider `std::pmr` for flexibility
- **Logging:**
  - Consider lock-free or buffered logging for high-frequency events
  - Allow runtime log level configuration
- **Benchmarking:**
  - Add multi-threaded benchmarks
  - Output results in machine-readable formats (CSV, JSON)
- **Modern C++:**
  - Use `constexpr`, `noexcept`, `[[nodiscard]]`, and smart pointers
  - Use `std::span` for batch operations (C++20)
- **Documentation & Testing:**
  - Add unit/integration tests (e.g., Google Test)
  - Expand API and threading documentation
- **Build System:**
  - Add sanitizer options and CI integration

## License
[MIT License](https://github.com/SourenaMOOSAVI/low-latency-cpp?tab=MIT-1-ov-file#readme)