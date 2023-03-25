#pragma once

#include <vector>
#include "OrderBookEntry.h"
#include "OrderBook.h"
#include "Wallet.h"
#include "../WalletBalances.h"
#include "../Offer.h"


class MerkelMain
{
    public:
        MerkelMain(std::string datasetName);
        /** Call this to start the sim */
        void init();

        //Made this public and into a Boolean. Returns true if we have reached the end of the orderBook.
        bool gotoNextTimeframe();

        //Made this public so that we could print the status of our wallet so we know how well our bot did. 
        void printWallet();

        WalletBalances getBalances();

        // This returns a vector of accepted offers.
        std::vector<Offer> getAcceptedOffers();
        std::vector<Offer> getBestOffers();
        // returns 0 if ok, non-zero if error
        int makeOffer(Offer& o);
        // REturns number of orders removed, should be 1
        int withdrawOffer(Offer& o);
        std::vector<Offer> getMyOffers();

        // Get the current (simulated) time as a number of seconds since the UNIX epoch.
        // We really don't want to be messing around with different time formats
        // so we'll just get it to a unix seconds-since-epoch as quickly as possible.
        // NOTE: This will be wrong but consistant if the data is given in a timezone other than UTC.
        std::uint64_t getCurrentTime();


    private: 
        void printMenu();
        void printHelp();
        void printMarketStats();
        void enterAsk();
        void enterBid();
        int getUserOption();
        void processUserOption(int userOption);
        Offer offerFromObe(OrderBookEntry& obe);
        OrderBookEntry obeFromOffer(Offer& o);

        std::string currentTime;

//        OrderBook orderBook{"20200317.csv"};
	    OrderBook orderBook;
        Wallet wallet;

        std::vector<Offer> acceptedOffers;
};
