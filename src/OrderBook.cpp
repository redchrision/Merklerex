#include "OrderBook.h"
#include "CSVReader.h"
#include <map>
#include <algorithm>
#include <iostream>
#include <iomanip>
#include <sstream>

/** construct, reading a csv data file */
OrderBook::OrderBook(std::string filename)
{
    // std::cout << "Hello" << std::endl;
    // std::cout << filename << std::endl;

    std::string lastOt = "";
    std::vector<OrderBookEntry> orders = CSVReader::readCSV(filename);

    // Precompute this on first load because walking the list takes forevah
    for (OrderBookEntry& e : orders) {
        if (e.timestamp != lastOt) {
            timeIndex.emplace(e.timestamp, times.size());
            times.push_back(e.timestamp);
            lastOt = e.timestamp;
        }
        prodMap[e.product] = true;
        try {
            ordersByTime[lastOt].push_back(e);
        } catch (...) {
            ordersByTime.emplace(lastOt, std::vector<OrderBookEntry>{e});
        }
    }
}

/** return vector of all know products in the dataset*/
std::vector<std::string> OrderBook::getKnownProducts()
{
    std::vector<std::string> products;
    for (auto const& e : prodMap) {
        products.push_back(e.first);
    }
    return products;
}

/** return vector of Orders according to the sent filters*/
std::vector<OrderBookEntry> OrderBook::getOrders(OrderBookType type, std::string product)
{
    std::vector<OrderBookEntry> orders_sub;
    for (OrderBookEntry& e : currentlyOpenOrders) {
        if (e.orderType == type && e.product == product) {
            orders_sub.push_back(e);
        }
    }
    return orders_sub;
}


double OrderBook::getHighPrice(std::vector<OrderBookEntry>& orders)
{
    double max = orders[0].price;
    for (OrderBookEntry& e : orders)
    {
        if (e.price > max)max = e.price;
    }
    return max;
}


double OrderBook::getLowPrice(std::vector<OrderBookEntry>& orders)
{
    double min = orders[0].price;
    for (OrderBookEntry& e : orders)
    {
        if (e.price < min)min = e.price;
    }
    return min;
}

std::string OrderBook::getEarliestTime()
{
    return times[0];
}

std::string OrderBook::getNextTime(std::string timestamp)
{
    int idx;
    try {
        idx = this->timeIndex.at(timestamp);
    } catch (...) {
        std::cout << "Reached the end at " << timestamp << "\n";
        return times[0];
    }
    try {
        return times.at(idx+1);
    } catch (...) {
        std::cout << "Reached the end at of " << timestamp << "\n";
        return times[0];
    }
}

std::vector<OrderBookEntry> OrderBook::getHistoricaOrders(std::string timestamp)
{
    return ordersByTime.at(timestamp);
}

void OrderBook::insertOrder(OrderBookEntry& order)
{
    currentlyOpenOrders.push_back(order);
}

int OrderBook::removeOrder(OrderBookEntry& order)
{
    auto isz = currentlyOpenOrders.size();
    currentlyOpenOrders.erase(
        std::remove_if(
            currentlyOpenOrders.begin(), 
            currentlyOpenOrders.end(),
            [o = std::move(order)](OrderBookEntry& x){ return x.equals(o); }),
        currentlyOpenOrders.end()
    );
    return isz - currentlyOpenOrders.size();
}

std::vector<OrderBookEntry> OrderBook::getMyOrders()
{
    std::vector<OrderBookEntry> out{};
    for (OrderBookEntry& obe : this->currentlyOpenOrders) {
        if (obe.username == "simuser") {
            out.push_back(obe);
        }
    }
    return out;
}

std::uint64_t OrderBook::tsToUnix(std::string ts)
{
    std::tm tm = {};
    // 2020/03/17 17:01:24.884492
    std::stringstream ss(ts.substr(0, ts.find('.')));
    ss >> std::get_time(&tm, "%Y/%m/%d %H:%M:%S");
    return std::mktime(&tm);
}

// Ok listen up, you can't do what you were doing here, not if you want accurate result.
// You're matching orders which are placed at exactly the same time, but orders placed
// a moment ago can linger for a while before another order comes in to take them.
//
// Doing this correctly is a fair bit more annoying, you need to implement a list of
// "currently open orders" and then add orders to that when they come in and then remove
// them when they're matched.
std::vector<OrderBookEntry> OrderBook::matchAsksToBids(std::string product)
{
    std::vector<OrderBookEntry> asks = getOrders(OrderBookType::ask, product);
    std::vector<OrderBookEntry> bids = getOrders(OrderBookType::bid, product);

    // sales = []
    std::vector<OrderBookEntry> sales; 

    // I put in a little check to ensure we have bids and asks
    // to process.
    if (asks.size() == 0 || bids.size() == 0)
    {
        //std::cout << " OrderBook::matchAsksToBids no bids or asks" << std::endl;
        return sales;
    }

    // sort asks lowest first
    std::sort(asks.begin(), asks.end(), OrderBookEntry::compareByPriceAsc);
    // sort bids highest first
    std::sort(bids.begin(), bids.end(), OrderBookEntry::compareByPriceDesc);
    // for ask in asks:
    //std::cout << "max ask " << asks[asks.size()-1].price << std::endl;
    //std::cout << "min ask " << asks[0].price << std::endl;
    //std::cout << "max bid " << bids[0].price << std::endl;
    //std::cout << "min bid " << bids[bids.size()-1].price << std::endl;
    
    for (OrderBookEntry& ask : asks)
    {
        // Filter out the zeros because we kill off bids and asks in-place
        if (ask.amount == 0) { continue; }
        for (OrderBookEntry& bid : bids)
        {
            if (ask.amount == 0) { break; }
            if (bid.amount == 0) { continue; }

            if (bid.price >= ask.price)
            {
                // The time of the sale is the latest time from ask and bid.
                std::string ts = bid.timestamp;
                if (tsToUnix(ask.timestamp) > tsToUnix(bid.timestamp)) {
                    ts = ask.timestamp;
                }
                OrderBookEntry sale{
                    ask.price,
                    std::min(ask.amount, bid.amount),
                    ts,
                    product,
                    OrderBookType::asksale,
                };
                if (sale.amount == 0) {
                    std::cout << " " << ask.amount << " " << bid.amount << " " << sale.amount << "\n";

                    throw new std::runtime_error("zero amount");
                }

                if (bid.username == "simuser") {
                    sale.username = "simuser";
                    sale.orderType = OrderBookType::bidsale;
                }
                if (ask.username == "simuser") {
                    sale.username = "simuser";
                    sale.orderType =  OrderBookType::asksale;
                }

                // The size of the sale is the MIN of the amount offered
                // and the amount wanted, if there is extra offered or wanted
                // then the offer can be processed again.
                bid.amount -= sale.amount;
                ask.amount -= sale.amount;
                sales.push_back(sale);
            }
        }
    }

    // Drop everything that matches our product
    currentlyOpenOrders.erase(
        std::remove_if(
            currentlyOpenOrders.begin(), 
            currentlyOpenOrders.end(),
            [p = move(product)](OrderBookEntry& x){ return x.product == p; }),
        currentlyOpenOrders.end()
    );

    // Finally, push all of the "surviving" orders
    for (OrderBookEntry& ask : asks) {
        if (ask.amount > 0) {
            currentlyOpenOrders.push_back(ask);
        }
    }
    for (OrderBookEntry& bid : bids) {
        if (bid.amount > 0) {
            currentlyOpenOrders.push_back(bid);
        }
    }

    return sales;             
}