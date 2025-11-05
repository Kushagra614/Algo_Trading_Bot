# HyperTradeX

A low-latency quantitative trading system for algorithmic trading with millisecond precision.

## 🚀 Key Features
- Real-time market data processing via WebSocket
- In-memory order book replication
- Modular strategy engine (SMA, Bollinger Bands, etc.)
- Asynchronous execution engine
- Risk management system
- Performance monitoring and metrics

## 🛠️ Tech Stack
- **Language**: C++20
- **Networking**: Boost.Asio, WebSocket++
- **Data**: nlohmann/json, flat_hash_map
- **Concurrency**: Lock-free data structures
- **Monitoring**: prometheus-cpp, spdlog
- **Build**: CMake

## 🚀 Quick Start
```bash
# Clone and build
mkdir build && cd build
cmake .. && make

# Run with config
./HyperTradeX config.json
```

## 📊 System Architecture
```
Market Data → Order Book → Strategy Engine → Execution Engine → Exchange
    ↑                            ↓                  ↓
    └─────── Risk Manager ←──────┘           Performance Metrics
```

## 📈 Supported Strategies
- SMA Crossover
- Bollinger Bands
- Momentum Burst
- (Extensible for custom strategies)

## 📝 License
MIT

---
Built with ❤️ by Kushagra Vardhan (DEATHAMBUSH)
