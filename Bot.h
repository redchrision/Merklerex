#pragma once
#include <string>
#include "Log.h"
#include "src/MerkelMain.h"
#include "Strategy.h"

class Bot
{
    public:
		Bot(MerkelMain& mm,
			Strategy& _strategy,
			Log& _balanceLog,
			Log& _offerLog,
			Log& _tradeLog
		);
	
		void cycle();

	private:
		MerkelMain& mm;
		Log& balanceLog;
		Log& offerLog;
		Log& tradeLog;
		WalletBalances currentBalances;
		Strategy& strategy;
};