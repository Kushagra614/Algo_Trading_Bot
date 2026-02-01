#include "../include/MarketData.h"
#include "../include/TechnicalAnalysisOptimized.h"
#include "../include/Benchmark.h"
#include <iostream>
#include <vector>

void runBenchmarks(const std::string& dataFile) {
    // Load market data
    std::cout << "\n=== Loading Market Data ===\n";
    MarketData data("AAPL");
    {
        TIME_SCOPE("Data Loading");
        if (!data.loadFromBinary(dataFile)) {
            std::cerr << "Failed to load market data from " << dataFile << "\n";
            return;
        }
    }
    
    std::cout << "\n=== Technical Analysis Benchmarks ===\n";
    
    // Initialize technical analysis
    TechnicalAnalysisOptimized ta;
    
    // Benchmark EMA calculation
    {
        TIME_SCOPE("50-period EMA Calculation");
        ta.calculateEMASIMD(data, 50);
    }
    
    // Benchmark RSI calculation
    {
        TIME_SCOPE("14-period RSI Calculation");
        ta.calculateRSI(data, 14);
    }
    
    // Benchmark MACD calculation
    {
        TIME_SCOPE("MACD (12,26,9) Calculation");
        ta.calculateMACD(data, 12, 26, 9);
    }
    
    std::cout << "\n=== Backtesting Benchmarks ===\n";
    
    // Benchmark backtesting
    {
        OptimizedBacktester backtester(std::thread::hardware_concurrency());
        TIME_SCOPE("Complete Backtest");
        backtester.runBacktest(data);
        
        // Print results
        auto metrics = backtester.getMetrics();
        std::cout << "\n=== Backtest Results ===\n";
        std::cout << "Total Return: " << metrics.totalReturn * 100 << "%\n";
        std::cout << "Annualized Return: " << metrics.annualizedReturn * 100 << "%\n";
        std::cout << "Sharpe Ratio: " << metrics.sharpeRatio << "\n";
        std::cout << "Max Drawdown: " << metrics.maxDrawdown * 100 << "%\n";
        std::cout << "Win Rate: " << metrics.winRate * 100 << "%\n";
        std::cout << "Total Trades: " << metrics.numTrades << "\n";
    }
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <market_data.bin>\n";
        std::cerr << "First run: ./json_to_binary data/AAPL_1y.json data/AAPL_1y.bin\n";
        return 1;
    }
    
    const std::string dataFile = argv[1];
    runBenchmarks(dataFile);
    
    return 0;
}
