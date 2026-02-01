#pragma once
#include "MarketData.h"
#include <vector>
#include <immintrin.h>
#include <thread>
#include <atomic>
#include <mutex>
#include <condition_variable>

class TechnicalAnalysisOptimized {
    alignas(64) std::vector<double> emaValues;
    alignas(64) std::vector<double> rsiValues;
    alignas(64) std::vector<double> macdLine;
    alignas(64) std::vector<double> signalLine;
    
    // Thread pool for parallel processing
    std::vector<std::thread> workers;
    std::mutex queueMutex;
    std::condition_variable condition;
    bool stop = false;
    
    // SIMD width in doubles (4 for AVX2, 2 for SSE4.2)
    static constexpr size_t SIMD_WIDTH = 4; // AVX2
    
    // Internal SIMD types
    using simd_double = double __attribute__((vector_size(32)));
    
public:
    TechnicalAnalysisOptimized();
    ~TechnicalAnalysisOptimized();
    
    // SIMD-optimized EMA calculation
    void calculateEMASIMD(const MarketData& data, size_t period);
    
    // Optimized RSI calculation with SIMD
    void calculateRSI(const MarketData& data, size_t period = 14);
    
    // Parallel MACD calculation
    void calculateMACD(const MarketData& data, int fast = 12, int slow = 26, int signal = 9);
    
    // Get results
    const std::vector<double>& getEMA() const { return emaValues; }
    const std::vector<double>& getRSI() const { return rsiValues; }
    
private:
    // Worker thread function
    void workerThread();
    
    // SIMD helper functions
    static inline double horizontalSum(const simd_double& v) {
        alignas(32) double tmp[SIMD_WIDTH];
        _mm256_store_pd(tmp, v);
        return tmp[0] + tmp[1] + tmp[2] + tmp[3];
    }
    
    // Parallel processing tasks
    struct Task {
        std::function<void()> work;
    };
    
    std::queue<Task> tasks;
};
