# High-Frequency Trading System

A C++ project to build a low-latency, high-performance high-frequency trading system, focusing on concurrency, lock-free programming, memory management, and networking.

## Project Overview
This project implements a multithreaded high-frequency trading (HFT) system. Week 1 focuses on a market data parser that reads mock data from a file and processes it using threads synchronized with mutexes.

## Setup Instructions
1. **Prerequisites**:
   - CMake (version 3.20 or higher)
   - C++20-compatible compiler (e.g., g++ 11 or later)
   - Git
2. **Clone the Repository**:
   ```bash
   git clone <repo-url>
   cd hft-system
   ```
3. **Build the Project**:
   ```bash
   mkdir build && cd build
   cmake ..
   make
   ```
4. **Run the Program**:
   ```bash
   ./hft_system
   ```
   The program reads mock market data from `data/mock_market_data.txt` and processes it for 5 seconds.

## Week 1: Multithreaded Market Data Parser
- **Functionality**: Reads mock market data (symbol, price, volume) from a file and processes it using two threads (reader and processor) with a mutex-protected queue.
- **Concepts Covered**: Threads (`std::thread`), mutexes (`std::mutex`), race conditions, thread safety.
- **Documentation**: See `docs/concurrency.md` for details on concurrency concepts.

## Repository Structure
- `src/`: Source code (main.cpp, market_data.h, market_data.cpp).
- `data/`: Mock market data file.
- `docs/`: Technical documentation.
- `CMakeLists.txt`: CMake build configuration.
- `.gitignore`: Ignores build artifacts and IDE files.