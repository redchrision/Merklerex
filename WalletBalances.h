#pragma once
#include <vector>
#include "WalletBalance.h"
#include "ChangeOfBalance.h"

// This class represents all the different currencies that the wallet has and the amount of each currency.

class WalletBalances
{
public:
	WalletBalances();
	void add(WalletBalance wb);
	std::vector<ChangeOfBalance> compareTo(WalletBalances& wbs);
	double get(std::string);

private:
	std::vector<WalletBalance> balances;
};
