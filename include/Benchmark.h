#pragma once
#include <chrono>
#include <string>
#include <iostream>

class ScopedTimer {
    using Clock = std::chrono::high_resolution_clock;
    std::string name;
    Clock::time_point start;
    bool stopped = false;
    
public:
    explicit ScopedTimer(std::string_view n) : name(n), start(Clock::now()) {}
    
    ~ScopedTimer() {
        if (!stopped) stop();
    }
    
    double stop() {
        auto end = Clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count() / 1e6;
        std::cout << "[" << name << "] " << duration << " ms\n";
        stopped = true;
        return duration;
    }
};

#define TIME_SCOPE(name) ScopedTimer timer_##__LINE__(name)
