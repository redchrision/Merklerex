#pragma once
#include <string>

// This class represents how many coins the wallet has of a particular currency. 

class WalletBalance
{
public:
	WalletBalance(double _balance,
		std::string _assetType);

	double balance;
	std::string assetType;

};