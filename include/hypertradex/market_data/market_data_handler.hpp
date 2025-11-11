#ifndef HYPERTRADEX_MARKET_DATA_HANDLER_HPP
#define HYPERTRADEX_MARKET_DATA_HANDLER_HPP

#include<string>
#include<vector>
#include<memory>
#include<functional>
#include<chrono>

using namespace  std;

//Basic Types
using Price = double;
using Quantity = double;
using Timestamp = chrono::nanoseconds;
using Symbol = string;


//Event Type
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
};

//Trade Event
struct TradeEvent : public MarketDataEvent {
    EventType type() const override { return EventType::TRADE; }
    Price price;
    Quantity quantity;
    bool is_buyer_maker;
    Symbol symbol;
};

struct PriceLevel {
    Price price;
    Quantity quantity;

    // For sorting (bids: highest first, asks: lowest first)
    bool operator<(const PriceLevel& other) const {
        return price <other.price;
    }
};

//orderbook update event
struct OrderBookUpdateEvent : public MarketDataEvent {
    EventType type () const override { return EventType::ORDER_BOOK_UPDATE; }
    Symbol symbol;
    vector<PriceLevel> bids;
    vector<PriceLevel> asks;
    bool is_snapshot;
};

struct ConnectionStatusEvent : public MarketDataEvent {
    EventType type() const override { return EventType::CONNECTION_STATUS;}
    bool is_connected;
    string message;
};