#pragma once
#include "Offer.h"
#include "WalletBalances.h"
#include "Actions.h"
#include <vector>

class Strategy
{
public:
	virtual Actions cycle(
		std::uint64_t currentTime,
		std::vector<Offer> myOffers,
		std::vector<Offer> bestOffers,
		WalletBalances myBalances) = 0;
};