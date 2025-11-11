#ifndef HYPERTRADEX_MARKET_DATA_IMARKET_DATA_HANDLER_HPP
#define HYPERTRADEX_MARKET_DATA_IMARKET_DATA_HANDLER_HPP

#include "hypertradex/market_data/market_date_handler.hpp"

namespace hypertradex {

    class IMarketDatahandler;
    using MarketDataCallback = function<void(const MarketDataEvent&)>;

    classIMarketDataHandler {
        public:
        virtual ~ImarketDataHandler = default;

        // Lifecycle Management
        virtual bool start() = 0;  // Start the market data feed
        virtual void stop() = 0;   // Stop the feed
        virtual bool is_running() const = 0;  // Check if running

        // Subscription Management
        virtual void subscribe(const Symbol& symbol) = 0;
        virtual void unsubscribe(const Symbol& symbol) = 0;

        virtual void register_callback(MarketDataCallback callback) = 0;

        virtual const vector<PriceLevel>& get_bids(const Symbol& symbol) const = 0;
        virtual const vector<PriceLevel>& get_asks(const Symbol& symbol) const = 0;

        virtual Price get_last_price(const Symbol& symbol) const = 0;
        
};

}

#endif