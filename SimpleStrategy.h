#pragma once
#include "Strategy.h"
#include "OffersAtTime.h"
#include <vector>
#include <queue>

/// <summary>
/// The SimpleStrategy class implements strategy and all it does is trade based on whether the current price
/// is higher or lower than the price TIMESPAN_SECONDS ago.
/// 
/// If the current price is more than TRIGGER_BUY (percent) higher than it was TIMESPAN_SECONDS ago and the
/// BTC value of the asset held is less than 25% of the current holding of BTC, then 5% of the current holding
// of BTC is allocated to buy.
/// 
/// If the current price is more than TRIGGER_SELL (percent) lower than it was 24 hours ago, we dump the asset.
/// 
/// If any orders are to be placed then all currently active orders are withdrawn in order to free up funds.
/// 
/// Also, in order to keep the simple strategy simple, we're only transacting in pairs with BTC.
/// If we're buying an asset with BTC, we spend 20% of our BTC, if we are selling an asset we dump all of it.
/// </summary>
class SimpleStrategy : public Strategy
{
public:
	SimpleStrategy() : historicalOffersByTime() {}
	virtual Actions cycle(
		std::uint64_t currentTime,
		std::vector<Offer> myOffers,
		std::vector<Offer> bestOffers,
		WalletBalances myBalances);

private:
	const uint64_t TIMESPAN_SECONDS = 60;
	const double TRIGGER_BUY = 102;
	const double TRIGGER_SELL = 90;
	/// <summary>
	/// The offers which existed at historical times less than 24 hours ago. Times are given in seconds
	/// since the UNIX epoch.
	/// </summary>
	std::queue<OffersAtTime> historicalOffersByTime;
};