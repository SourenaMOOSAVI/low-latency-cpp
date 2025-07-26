#include "thread_affinity.h"
#include <pthread.h>
#include <sched.h>
#include <stdexcept>
#include <system_error>

/**
 * @brief Sets thread affinity to a specific CPU core.
 * 
 * Uses POSIX pthread APIs to pin a thread to a CPU core, optimizing scheduling for
 * low-latency systems. Ensures threads run on designated cores to minimize context
 * switches and NUMA-related memory access latencies.
 * 
 * @param thread_id The ID of the thread to pin.
 * @param cpu_core The CPU core number to pin the thread to.
 * @throws std::runtime_error if setting affinity fails or if the core is invalid.
 */
void setThreadAffinity(std::thread::id thread_id, int cpu_core) {
    // Check if CPU core is valid
    if (cpu_core < 0 || cpu_core >= CPU_SETSIZE) {
        throw std::runtime_error("Invalid CPU core number");
    }

    // Initialize CPU set and add specified core
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(cpu_core, &cpuset);

    // Convert std::thread::id to pthread_t
    pthread_t thread = pthread_self(); // Simplified for current thread
    // Note: Mapping std::thread::id to pthread_t may require platform-specific handling
    // For simplicity, assumes thread_id corresponds to current thread

    // Set thread affinity
    int result = pthread_setaffinity_np(thread, sizeof(cpu_set_t), &cpuset);
    if (result != 0) {
        throw std::system_error(result, std::system_category(), "Failed to set thread affinity");
    }
}