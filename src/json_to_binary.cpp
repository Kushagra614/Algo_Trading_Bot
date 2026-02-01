#include "../include/MarketData.h"
#include <fstream>
#include <nlohmann/json.hpp>
#include <iostream>

using json = nlohmann::json;

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <input.json> <output.bin>\n";
        return 1;
    }

    const std::string inputFile = argv[1];
    const std::string outputFile = argv[2];

    // Read JSON file
    std::ifstream inFile(inputFile);
    if (!inFile) {
        std::cerr << "Failed to open input file: " << inputFile << "\n";
        return 1;
    }

    // Parse JSON
    json j;
    try {
        inFile >> j;
    } catch (const std::exception& e) {
        std::cerr << "JSON parse error: " << e.what() << "\n";
        return 1;
    }

    // Create market data
    MarketData marketData("converted");
    
    // Reserve space for all ticks
    std::vector<MarketTick> ticks;
    ticks.reserve(j.size());

    // Convert JSON to MarketTick
    for (const auto& item : j) {
        MarketTick tick;
        try {
            tick.timestamp = std::stoll(item["timestamp"].get<std::string>());
            tick.price = std::stod(item["close"].get<std::string>());
            tick.volume = std::stod(item["volume"].get<std::string>());
            ticks.push_back(tick);
        } catch (const std::exception& e) {
            std::cerr << "Error processing tick: " << e.what() << "\n";
            continue;
        }
    }

    // Save to binary
    std::ofstream outFile(outputFile, std::ios::binary);
    if (!outFile) {
        std::cerr << "Failed to open output file: " << outputFile << "\n";
        return 1;
    }

    outFile.write(reinterpret_cast<const char*>(ticks.data()), 
                 ticks.size() * sizeof(MarketTick));

    if (!outFile) {
        std::cerr << "Error writing to output file\n";
        return 1;
    }

    std::cout << "Successfully converted " << ticks.size() 
              << " ticks to binary format.\n";
    return 0;
}
