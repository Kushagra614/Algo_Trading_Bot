#pragma once
#include "MarketData.h"
#include "TechnicalAnalysisOptimized.h"
#include <vector>
#include <atomic>
#include <memory>
#include <mutex>

// Lock-free queue for order processing
template<typename T, size_t Size>
class LockFreeQueue {
    alignas(64) std::atomic<size_t> head{0};
    alignas(64) std::atomic<size_t> tail{0};
    alignas(64) T buffer[Size];
    
public:
    bool try_enqueue(T&& item) {
        size_t current_tail = tail.load(std::memory_order_relaxed);
        size_t next_tail = (current_tail + 1) % Size;
        
        if (next_tail == head.load(std::memory_order_acquire))
            return false;
            
        buffer[current_tail] = std::move(item);
        tail.store(next_tail, std::memory_order_release);
        return true;
    }
    
    bool try_dequeue(T& item) {
        size_t current_head = head.load(std::memory_order_relaxed);
        if (current_head == tail.load(std::memory_order_acquire))
            return false;
            
        item = std::move(buffer[current_head]);
        head.store((current_head + 1) % Size, std::memory_order_release);
        return true;
    }
};

struct Trade {
    int64_t entryTime;
    double entryPrice;
    double exitPrice;
    double positionSize;
    double pnl;
    double pnlPct;
    bool isLong;
};

class OptimizedBacktester {
    struct alignas(64) ThreadLocalData {
        std::vector<double> indicators;
        std::vector<Trade> trades;
        double equity = 0;
        double maxDrawdown = 0;
    };
    
    std::vector<ThreadLocalData> threadData;
    TechnicalAnalysisOptimized ta;
    LockFreeQueue<Trade, 1024> tradeQueue;
    std::atomic<bool> stopRequested{false};
    
    // Performance metrics
    alignas(64) std::atomic<uint64_t> totalTrades{0};
    alignas(64) std::atomic<double> totalPnl{0};
    alignas(64) std::atomic<double> maxDrawdown{0};
    
public:
    OptimizedBacktester(size_t numThreads = std::thread::hardware_concurrency());
    ~OptimizedBacktester();
    
    // Run backtest in parallel
    void runBacktest(const MarketData& data);
    
    // Get performance metrics
    struct Metrics {
        double totalReturn;
        double annualizedReturn;
        double sharpeRatio;
        double maxDrawdown;
        double winRate;
        uint64_t numTrades;
    };
    
    Metrics getMetrics() const;
    
private:
    void processChunk(size_t startIdx, size_t endIdx, size_t threadId);
    void processTrades();
    
    // Thread management
    std::vector<std::thread> workers;
    std::mutex statsMutex;
    std::condition_variable cv;
    
    // Trading strategy (example: moving average crossover)
    bool shouldEnterLong(size_t idx, const MarketData& data, const TechnicalAnalysisOptimized& ta) const;
    bool shouldExitLong(size_t idx, const MarketData& data, const TechnicalAnalysisOptimized& ta) const;
};
