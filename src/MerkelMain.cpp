#include "MerkelMain.h"
#include <iostream>
#include <vector>
#include "OrderBookEntry.h"
#include "CSVReader.h"

MerkelMain::MerkelMain(std::string datasetname) :
    orderBook{datasetname}
{
}

void MerkelMain::init()
{
    //int input;
    currentTime = orderBook.getEarliestTime();

    wallet.insertCurrency("BTC", 10);
    
    std::cout << "Hello 1" << std::endl;

    //This section was commented out beacause init needs to hand control back so that the bot could be run. 
    /* 
    while(true)
    {
        printMenu();
        input = getUserOption();
        processUserOption(input);
    }*/
}


void MerkelMain::printMenu()
{
    // 1 print help
    std::cout << "1: Print help " << std::endl;
    // 2 print exchange stats
    std::cout << "2: Print exchange stats" << std::endl;
    // 3 make an offer
    std::cout << "3: Make an offer " << std::endl;
    // 4 make a bid 
    std::cout << "4: Make a bid " << std::endl;
    // 5 print wallet
    std::cout << "5: Print wallet " << std::endl;
    // 6 continue   
    std::cout << "6: Continue " << std::endl;

    std::cout << "============== " << std::endl;

    std::cout << "Current time is: " << currentTime << std::endl;
}

void MerkelMain::printHelp()
{
    std::cout << "Help - your aim is to make money. Analyse the market and make bids and offers. " << std::endl;
}

void MerkelMain::printMarketStats()
{
    for (std::string const& p : orderBook.getKnownProducts())
    {
        std::cout << "Product: " << p << std::endl;
        std::vector<OrderBookEntry> entries = orderBook.getOrders(OrderBookType::ask, p);
        std::cout << "Asks seen: " << entries.size() << std::endl;
        std::cout << "Max ask: " << OrderBook::getHighPrice(entries) << std::endl;
        std::cout << "Min ask: " << OrderBook::getLowPrice(entries) << std::endl;



    }
    // std::cout << "OrderBook contains :  " << orders.size() << " entries" << std::endl;
    // unsigned int bids = 0;
    // unsigned int asks = 0;
    // for (OrderBookEntry& e : orders)
    // {
    //     if (e.orderType == OrderBookType::ask)
    //     {
    //         asks ++;
    //     }
    //     if (e.orderType == OrderBookType::bid)
    //     {
    //         bids ++;
    //     }  
    // }    
    // std::cout << "OrderBook asks:  " << asks << " bids:" << bids << std::endl;

}

void MerkelMain::enterAsk()
{
    std::cout << "Make an ask - enter the amount: product,price, amount, eg  ETH/BTC,200,0.5" << std::endl;
    std::string input;
    std::getline(std::cin, input);

    std::vector<std::string> tokens = CSVReader::tokenise(input, ',');
    if (tokens.size() != 3)
    {
        std::cout << "MerkelMain::enterAsk Bad input! " << input << std::endl;
    }
    else {
        try {
            OrderBookEntry obe = CSVReader::stringsToOBE(
                tokens[1],
                tokens[2], 
                currentTime, 
                tokens[0], 
                OrderBookType::ask 
            );
            obe.username = "simuser";
            if (wallet.canFulfillOrder(obe))
            {
                std::cout << "Wallet looks good. " << std::endl;
                orderBook.insertOrder(obe);
            }
            else {
                std::cout << "Wallet has insufficient funds . " << std::endl;
            }
        }catch (const std::exception& e)
        {
            std::cout << " MerkelMain::enterAsk Bad input " << std::endl;
        }   
    }
}

void MerkelMain::enterBid()
{
    std::cout << "Make an bid - enter the amount: product,price, amount, eg  ETH/BTC,200,0.5" << std::endl;
    std::string input;
    std::getline(std::cin, input);

    std::vector<std::string> tokens = CSVReader::tokenise(input, ',');
    if (tokens.size() != 3)
    {
        std::cout << "MerkelMain::enterBid Bad input! " << input << std::endl;
    }
    else {
        try {
            OrderBookEntry obe = CSVReader::stringsToOBE(
                tokens[1],
                tokens[2], 
                currentTime, 
                tokens[0], 
                OrderBookType::bid 
            );
            obe.username = "simuser";

            if (wallet.canFulfillOrder(obe))
            {
                std::cout << "Wallet looks good. " << std::endl;
                orderBook.insertOrder(obe);
            }
            else {
                std::cout << "Wallet has insufficient funds . " << std::endl;
            }
        }catch (const std::exception& e)
        {
            std::cout << " MerkelMain::enterBid Bad input " << std::endl;
        }   
    }
}

void MerkelMain::printWallet()
{
    std::cout << wallet.toString() << std::endl;
}

WalletBalances MerkelMain::getBalances()
{
    return wallet.getBalances();
}

std::vector<Offer> MerkelMain::getAcceptedOffers()
{
    std::vector<Offer> out(this->acceptedOffers);
    this->acceptedOffers.clear();
    return out;
}

std::vector<Offer> MerkelMain::getBestOffers()
{
    std::vector<Offer> out{};
    for (std::string const& p : orderBook.getKnownProducts()) {
        std::vector<OrderBookEntry> asks = this->orderBook.getOrders(OrderBookType::ask, p);
        if (asks.size() > 0) {
            OrderBookEntry& best = asks[0];
            for (OrderBookEntry& ask : asks) {
                if (ask.price < best.price) {
                    best = ask;
                }
            }
            out.push_back(this->offerFromObe(best));
        }
        std::vector<OrderBookEntry> bids = this->orderBook.getOrders(OrderBookType::bid, p);
        if (bids.size() > 0) {
            OrderBookEntry& best = bids[0];
            for (OrderBookEntry& bid : bids) {
                if (bid.price < best.price) {
                    best = bid;
                }
            }
            out.push_back(this->offerFromObe(best));
        }
    }
    return out;
}

int MerkelMain::makeOffer(Offer& o)
{
    OrderBookEntry obe = this->obeFromOffer(o);
    if (wallet.canFulfillOrder(obe)) {
        orderBook.insertOrder(obe);
    } else {
        std::cout << "Wallet has insufficient funds . " << std::endl;
        return 100;
    }
    return 0;
}

int MerkelMain::withdrawOffer(Offer& o)
{
    OrderBookEntry obe = this->obeFromOffer(o);
    return orderBook.removeOrder(obe);
}

std::vector<Offer> MerkelMain::getMyOffers()
{
    std::vector<Offer> out{};
    for (OrderBookEntry& obe: this->orderBook.getMyOrders()) {
        out.push_back(this->offerFromObe(obe));
    }
    return out;
}

std::uint64_t MerkelMain::getCurrentTime()
{
    return OrderBook::tsToUnix(currentTime);
}

Offer MerkelMain::offerFromObe(OrderBookEntry& obe) {
    std::vector<std::string> wantOffer = CSVReader::tokenise(obe.product, '/');
    std::string want = wantOffer[0];
    std::string offer = wantOffer[1];
    if (obe.orderType == OrderBookType::ask || obe.orderType == OrderBookType::asksale) {
        want = wantOffer[1];
        offer = wantOffer[0];
    }
    double amountOffered = obe.amount * obe.price;
    double amountWanted = obe.amount;
    return Offer{
        amountOffered,
        offer,
        amountWanted,
        want,
    };
}

static OrderBookEntry obeFromOfferPair(Offer& o, std::string pair, std::string currentTime, bool isAsk) {
    double price = 0;
    double amount = 0;
    OrderBookType type = OrderBookType::bid;
    if (isAsk) {
        amount = o.amountOffered;
        price = o.amountWanted / o.amountOffered;
        type = OrderBookType::ask;
    }
    else {
        amount = o.amountWanted;
        price = o.amountOffered / o.amountWanted;
    }
    return OrderBookEntry{ price, amount, currentTime, pair, type, "simuser" };
}

OrderBookEntry MerkelMain::obeFromOffer(Offer& o)
{
    std::string pair = o.typeWanted + "/" + o.typeOffered;
    for (std::string const& p : orderBook.getKnownProducts()) {
        if (p == pair) {
            return obeFromOfferPair(o, p, currentTime, false);
        }
    }
    std::string pair2 = o.typeOffered + "/" + o.typeWanted;
    for (std::string const& p : orderBook.getKnownProducts()) {
        if (p == pair2) {
            return obeFromOfferPair(o, p, currentTime, true);
        }
    }
    throw "no such asset pair " + pair + " or " + pair2;
}


// Changed this function into a Boolean. Returns true if we have reached the end of the orderBook. 

bool MerkelMain::gotoNextTimeframe()
{
    //std::cout << "Going to next time frame. " << std::endl;
    for (std::string p : orderBook.getKnownProducts())
    {
        //std::cout << "matching " << p << std::endl;
        std::vector<OrderBookEntry> sales =  orderBook.matchAsksToBids(p);
        //std::cout << "Sales: " << sales.size() << std::endl;
        for (OrderBookEntry& sale : sales)
        {
            //std::cout << "Sale price: " << sale.price << " amount " << sale.amount << std::endl; 
            if (sale.username == "simuser")
            {
                // update the wallet
                wallet.processSale(sale);
                this->acceptedOffers.push_back(offerFromObe(sale));
            }
        }
        
    }

    currentTime = orderBook.getNextTime(currentTime);

    for (OrderBookEntry& e : orderBook.getHistoricaOrders(currentTime)) {
        orderBook.insertOrder(e);
    }

    return currentTime == orderBook.getEarliestTime();
}
 
int MerkelMain::getUserOption()
{
    int userOption = 0;
    std::string line;
    std::cout << "Type in 1-6" << std::endl;
    std::getline(std::cin, line);
    try{
        userOption = std::stoi(line);
    }catch(const std::exception& e)
    {
       
    }
    std::cout << "You chose: " << userOption << std::endl;
    return userOption;
}

void MerkelMain::processUserOption(int userOption)
{
    if (userOption == 0) // bad input
    {
        std::cout << "Invalid choice. Choose 1-6" << std::endl;
    }
    if (userOption == 1) 
    {
        printHelp();
    }
    if (userOption == 2) 
    {
        printMarketStats();
    }
    if (userOption == 3) 
    {
        enterAsk();
    }
    if (userOption == 4) 
    {
        enterBid();
    }
    if (userOption == 5) 
    {
        printWallet();
    }
    if (userOption == 6) 
    {
        gotoNextTimeframe();
    }       
}