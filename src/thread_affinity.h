#pragma once
#include <thread>
#include <stdexcept>

/**
 * @brief Sets thread affinity to a specific CPU core.
 * 
 * Pins a thread to a CPU core to reduce context switches and optimize NUMA-aware scheduling
 * in low-latency systems. Enhances predictability by minimizing cross-core migrations.
 * 
 * @param thread_id The ID of the thread to pin.
 * @param cpu_core The CPU core number to pin the thread to.
 * @throws std::runtime_error if setting affinity fails.
 */
void setThreadAffinity(std::thread::id thread_id, int cpu_core);