#include "Wallet.h"
#include <iostream>
#include "MerkelMain.h"
#include <filesystem>
#include "../Log.h"
#include "../Bot.h"
#include "../SimpleStrategy.h"


int main()
{   
    std::cout << "Current path is " << std::filesystem::current_path() << '\n';
    MerkelMain app{ "20200601.csv" };
    app.init();
    Log balanceLog{"balaceLog.csv"};
    Log offerLog{"offerLog.csv"};
    Log tradeLog{"tradeLog.csv"};

    SimpleStrategy st{};

    Bot bot{
        app,
        st,
        balanceLog,
        offerLog,
        tradeLog,
    };

    while (!app.gotoNextTimeframe()) {
        bot.cycle();
    }

    app.printWallet();

    std::cout << "Hello 7" << std::endl;

    std::string line;
    std::cout << "Press ENTER. We are done." << std::endl;
    std::getline(std::cin, line);

}