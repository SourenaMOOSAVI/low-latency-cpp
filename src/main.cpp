#include "market_data.h"
#include <iostream>

/**
 * @brief Entry point for the low-latency system demonstration.
 * 
 * Initializes a MarketDataParser, starts producer and consumer threads, and waits for user input
 * to stop the system. Demonstrates concurrency and low-latency design principles.
 */
int main() {
    std::cout << "Starting HFT system\n";
    MarketDataParser parser;
    parser.start();
    std::cin.get(); // Wait for Enter
    parser.stop();
    std::cout << "HFT system stopped\n";
    return 0;
}