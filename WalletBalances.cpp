#include "WalletBalances.h"

WalletBalances::WalletBalances()
{
}

void WalletBalances::add(WalletBalance wb)
{
	this->balances.push_back(wb);
}

// This function used an algorithm which will compare the balance of the wallet from one moment to another, 
// in order to perform the loging requirement every time there is a change to the wallet. 

std::vector<ChangeOfBalance> WalletBalances::compareTo(WalletBalances& wbs)
{
	std::vector<ChangeOfBalance> out;

	for (WalletBalance& bal : this->balances)
	{
		bool found = false;
		for (WalletBalance& oldBal : wbs.balances)
		{
			if (bal.assetType == oldBal.assetType)
			{
				if (bal.balance != oldBal.balance)
				{
					out.push_back(ChangeOfBalance{ bal.assetType, oldBal.balance, bal.balance });
				}
				found = true;
				break;
			}
		}
		if (found == false)
		{
			out.push_back(ChangeOfBalance{ bal.assetType, 0, bal.balance });
		}
	}

	for (WalletBalance& oldBal : wbs.balances)
	{
		bool found = false;
		for (WalletBalance& bal : this->balances)
		{
			if (bal.assetType == oldBal.assetType)
			{
				found = true;
				break;
			}
		}
		if (found == false)
		{
			out.push_back(ChangeOfBalance{ oldBal.assetType, oldBal.balance, 0 });
		}
	}

	return out;
}

double WalletBalances::get(std::string currency)
{
	for (WalletBalance& wb : this->balances) {
		if (wb.assetType == currency) {
			return wb.balance;
		}
	}
	return 0;
}
