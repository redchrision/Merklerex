#pragma once
#include "OrderBookEntry.h"
#include "CSVReader.h"
#include <string>
#include <vector>
#include <map>

class OrderBook
{
    public:
    /** construct, reading a csv data file */
        OrderBook(std::string filename);
    /** return vector of all know products in the dataset*/
        std::vector<std::string> getKnownProducts();
    /** return vector of Orders according to the sent filters*/
        std::vector<OrderBookEntry> getOrders(OrderBookType type, std::string product);

        /** returns the earliest time in the orderbook*/
        std::string getEarliestTime();
        /** returns the next time after the 
         * sent time in the orderbook  
         * If there is no next timestamp, wraps around to the start
         * */
        std::string getNextTime(std::string timestamp);

        void insertOrder(OrderBookEntry& order);

        // Returns the number of orders removed, removes all orders deemed equal (see OrderBookEntry::equal())
        int removeOrder(OrderBookEntry& obe);

        std::vector<OrderBookEntry> getMyOrders();

        std::vector<OrderBookEntry> matchAsksToBids(std::string product);

        std::vector<OrderBookEntry> getHistoricaOrders(std::string timestamp);

        static double getHighPrice(std::vector<OrderBookEntry>& orders);
        static double getLowPrice(std::vector<OrderBookEntry>& orders);
        static std::uint64_t tsToUnix(std::string ts);

    private:
        std::string currentTime;
        std::vector<std::string> times;
        std::map<std::string,int> timeIndex;
        std::map<std::string,std::vector<OrderBookEntry>> ordersByTime;
        std::map<std::string,bool> prodMap;

        std::vector<OrderBookEntry> currentlyOpenOrders;


};
