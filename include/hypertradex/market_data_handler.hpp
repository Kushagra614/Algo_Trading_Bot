#ifndef HYPERTRADEX_MARKET_DATA_HANDLER_HPP
#define HYPERTRADEX_MARKET_DATA_HANDLER_HPP

#include<string>
#include<vector>
#include<memory>
#include<fucntional>
#include<chrono>

using namespace  std;

//Basic Types
using Price = double;
using Quantity = double;
using Timestamp = chrono::nanoseconds;
using Symbol = string;


//Event Types

enum class EventType {
    TRADE,
    ORDER_BOOK_UPDATE,
    CONNECTION_STATUS
};

// Base class for all market data events
struct MarketDataEvent {
    virtual ~MarketDataEvent() = default;
    virtual EventType type() const = 0;
    Timestamp exchange_timestamp;
    Timestamp local_timestamp;
}

//Trade Event

