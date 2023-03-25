#include "SimpleStrategy.h"

#include <unordered_map>
#include <iostream>

Actions SimpleStrategy::cycle(
	std::uint64_t currentTime,
	std::vector<Offer> myOffers,
	std::vector<Offer> bestOffers,
	WalletBalances myBalances)
{
	if (bestOffers.size() == 0) {
		return Actions{};
	}

	// 1. push the current offers to the queue
	historicalOffersByTime.push(OffersAtTime{currentTime,bestOffers});

	// 2. see if the oldest present data is too old
	OffersAtTime& bk = this->historicalOffersByTime.front();
	if (bk.time + TIMESPAN_SECONDS > currentTime) {
		return Actions{};
	}

	// 3. Pop everything which is too old
	while (this->historicalOffersByTime.size() > 0) {
		bk = this->historicalOffersByTime.front();
		if (bk.time + TIMESPAN_SECONDS > currentTime) {
			// found something which is not too old
			break;
		}
		this->historicalOffersByTime.pop();
	}

	std::cout << currentTime << " cycle\n";

	// 4. find oldprice/newprice for each btc-related asset
	std::unordered_map<std::string, double> old_prices;
	for (Offer& o : bk.offers) {
		if (o.typeOffered == "BTC") {
			// they're buying with btc
			double price = o.amountWanted / o.amountOffered;
			old_prices.emplace(o.typeWanted, price);
		}
		else if (o.typeWanted == "BTC") {
			// they're selling and they want btc
			double price = o.amountOffered / o.amountWanted;
			old_prices.emplace(o.typeOffered, price);
		}
	}
	std::unordered_map<std::string, double> price_changes;
	std::unordered_map<std::string, double> new_prices;
	for (Offer& o : bestOffers) {
		double price = 0;
		std::string asset = "";
		if (o.typeOffered == "BTC") {
			// buying with btc
			price = o.amountWanted / o.amountOffered;
			asset = o.typeWanted;
		}
		else if (o.typeWanted == "BTC") {
			// selling for btc
			price = o.amountWanted / o.amountOffered;
			asset = o.amountOffered;
		}
		else {
			continue;
		}
		new_prices.emplace(asset, price);
		try {
			double oldprice = old_prices.at(asset);
			price_changes.emplace(asset, price / oldprice);
		}
		catch (...) {}
	}

	// 5. figure out what we should sell first
	Actions out;
	for (auto & apc : price_changes) {
		if (apc.second > TRIGGER_SELL/100) { continue;  }
		double bal = myBalances.get(apc.first);
		double cur_price = new_prices.at(apc.first);
		// dump all of it
		out.offersToMake.push_back(Offer{ bal, apc.first, bal * cur_price, "BTC" });
		std::cout << currentTime << " SELL " << bal << " " << apc.first << " FOR " <<
			bal * cur_price << "\n";
	}

	double availableBtc = myBalances.get("BTC");

	// 6. Place orders on all of the assets which are worth buying
	for (auto& apc : price_changes) {
		if (apc.second < TRIGGER_BUY/100) { continue; }
		double assetBal = myBalances.get(apc.first);
		double cur_price = new_prices.at(apc.first);
		if (assetBal * cur_price > availableBtc * 0.25) {
			// We're already holding a bunch of this asset, lets not buy any more
			continue;
		}
		double btcToSpend = availableBtc * 0.05;
		double amtToRequest = btcToSpend / cur_price;
		out.offersToMake.push_back(Offer{ btcToSpend, "BTC", amtToRequest, apc.first });
		std::cout << currentTime << " BUY " << amtToRequest <<
			" " << apc.first << " FOR " << btcToSpend << "\n";
	}

	// 7. If we're making any orders then lets yank all of the orders which are outstanding
	if (out.offersToMake.size() > 0) {
		for (Offer o : myOffers) {
			out.offersToWithdraw.push_back(o);
		}
	}
	return out;
}