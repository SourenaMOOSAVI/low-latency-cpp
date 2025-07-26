#pragma once
#include <string>

struct alignas(64) MarketData {
    std::string symbol; // ~24 bytes (implementation-dependent)
    double price;       // 8 bytes
    int volume;         // 4 bytes
    char padding[20];   // Pad to 64 bytes (assuming sizeof(std::string) ≈ 24)
};