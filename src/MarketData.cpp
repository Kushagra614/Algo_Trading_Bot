#include "../include/MarketData.h"
#include "../include/Benchmark.h"
#include <fstream>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>

bool MarketData::loadFromBinary(const std::string& filename) {
    TIME_SCOPE("MarketData::loadFromBinary");
    
    int fd = open(filename.c_str(), O_RDONLY);
    if (fd == -1) {
        std::cerr << "Failed to open file: " << filename << " (" << strerror(errno) << ")\n";
        return false;
    }
    
    // Get file size
    off_t file_size = lseek(fd, 0, SEEK_END);
    if (file_size == -1) {
        close(fd);
        return false;
    }
    lseek(fd, 0, SEEK_SET);
    
    // Memory map the file
    void* mapped = mmap(nullptr, file_size, PROT_READ, MAP_PRIVATE | MAP_POPULATE, fd, 0);
    if (mapped == MAP_FAILED) {
        std::cerr << "Memory mapping failed: " << strerror(errno) << "\n";
        close(fd);
        return false;
    }
    
    // Advise kernel about sequential access
    madvise(mapped, file_size, MADV_SEQUENTIAL);
    
    // Cast to our tick structure
    const char* data = static_cast<const char*>(mapped);
    const size_t num_ticks = file_size / sizeof(MarketTick);
    
    // Reserve space and copy with prefetching
    ticks.clear();
    ticks.reserve(num_ticks);
    
    // Prefetching parameters
    const size_t prefetch_distance = 16;  // Cache lines to prefetch ahead
    const size_t cache_line_ticks = CACHE_LINE_SIZE / sizeof(MarketTick);
    
    // Copy with prefetching
    for (size_t i = 0; i < num_ticks; ++i) {
        // Prefetch ahead
        if (i + prefetch_distance * cache_line_ticks < num_ticks) {
            __builtin_prefetch(data + (i + prefetch_distance * cache_line_ticks) * sizeof(MarketTick), 0, 1);
        }
        
        // Copy the tick
        ticks.push_back(*reinterpret_cast<const MarketTick*>(data + i * sizeof(MarketTick)));
    }
    
    // Cleanup
    munmap(mapped, file_size);
    close(fd);
    
    std::cout << "Loaded " << ticks.size() << " ticks from " << filename << "\n";
    return true;
}

bool MarketData::saveToBinary(const std::string& filename) const {
    std::ofstream out(filename, std::ios::binary | std::ios::trunc);
    if (!out) return false;
    
    out.write(reinterpret_cast<const char*>(ticks.data()), 
             ticks.size() * sizeof(MarketTick));
    return out.good();
}

double MarketData::calculateVWAP(size_t period) const {
    if (ticks.empty() || period == 0) return 0.0;
    
    double total_volume = 0.0;
    double sum_price_volume = 0.0;
    
    const size_t n = std::min(period, ticks.size());
    
    // Process in chunks for better cache utilization
    const size_t chunk_size = 1024;
    alignas(64) double chunk_volume[chunk_size];
    alignas(64) double chunk_price_volume[chunk_size];
    
    for (size_t i = 0; i < n; i += chunk_size) {
        size_t chunk_end = std::min(i + chunk_size, n);
        size_t chunk_len = chunk_end - i;
        
        // Process chunk
        for (size_t j = 0; j < chunk_len; ++j) {
            const auto& tick = ticks[i + j];
            chunk_volume[j] = tick.volume;
            chunk_price_volume[j] = tick.price * tick.volume;
        }
        
        // Sum chunk
        for (size_t j = 0; j < chunk_len; ++j) {
            total_volume += chunk_volume[j];
            sum_price_volume += chunk_price_volume[j];
        }
    }
    
    return (total_volume > 0) ? (sum_price_volume / total_volume) : 0.0;
}

void MarketData::calculateEMA(double* output, size_t period) const {
    if (ticks.empty() || period == 0) return;
    
    const double multiplier = 2.0 / (period + 1);
    output[0] = ticks[0].price;
    
    // Process with loop unrolling
    size_t i = 1;
    for (; i + 3 < ticks.size(); i += 4) {
        output[i] = (ticks[i].price - output[i-1]) * multiplier + output[i-1];
        output[i+1] = (ticks[i+1].price - output[i]) * multiplier + output[i];
        output[i+2] = (ticks[i+2].price - output[i+1]) * multiplier + output[i+1];
        output[i+3] = (ticks[i+3].price - output[i+2]) * multiplier + output[i+2];
    }
    
    // Handle remaining elements
    for (; i < ticks.size(); ++i) {
        output[i] = (ticks[i].price - output[i-1]) * multiplier + output[i-1];
    }
}
