#pragma once
#include <vector>
#include <string>
#include <cstdint>
#include <immintrin.h>
#include <memory>

// Cache line size for alignment
constexpr size_t CACHE_LINE_SIZE = 64;

// Custom allocator for aligned memory
template <typename T, size_t Alignment>
class aligned_allocator {
public:
    using value_type = T;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;

    aligned_allocator() = default;
    template <class U>
    aligned_allocator(const aligned_allocator<U, Alignment>&) {}

    T* allocate(std::size_t n) {
        if (n > std::numeric_limits<std::size_t>::max() / sizeof(T))
            throw std::bad_alloc();
        
        void* ptr = aligned_alloc(Alignment, n * sizeof(T));
        if (!ptr)
            throw std::bad_alloc();
            
        return static_cast<T*>(ptr);
    }

    void deallocate(T* p, std::size_t) {
        free(p);
    }
};

// Optimized data point structure
struct alignas(CACHE_LINE_SIZE) MarketTick {
    double price;
    double volume;
    int64_t timestamp;
    // Padding to fill cache line
    char padding[CACHE_LINE_SIZE - (2 * sizeof(double) + sizeof(int64_t)) % CACHE_LINE_SIZE];
};
static_assert(sizeof(MarketTick) % CACHE_LINE_SIZE == 0, "MarketTick not cache line aligned");

class MarketData {
    std::vector<MarketTick, aligned_allocator<MarketTick, CACHE_LINE_SIZE>> ticks;
    std::string symbol;
    
public:
    explicit MarketData(const std::string& sym) : symbol(sym) {
        ticks.reserve(1'000'000);  // Pre-allocate for 1M ticks
    }
    
    // Load from binary file
    bool loadFromBinary(const std::string& filename);
    
    // Save to binary format
    bool saveToBinary(const std::string& filename) const;
    
    // Accessors
    const MarketTick& operator[](size_t idx) const { return ticks[idx]; }
    size_t size() const { return ticks.size(); }
    
    // SIMD-optimized operations
    double calculateVWAP(size_t period) const;
    void calculateEMA(double* output, size_t period) const;
};
