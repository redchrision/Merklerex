#include "Bot.h"
#include "Offer.h"

Bot::Bot(MerkelMain& _mm,
	Strategy& _strategy,
	Log& _balanceLog,
	Log& _offerLog,
	Log& _tradeLog)
	: mm(_mm),
	strategy(_strategy),
	balanceLog(_balanceLog),
	offerLog(_offerLog),
	tradeLog(_tradeLog)
{
	this->tradeLog.log(Offer::CSVHeader());
}

void Bot::cycle()
{ 
	//This calls getBalances and stores the result in a variable called newBallances in order to get the balances. 
	WalletBalances newBalances = this->mm.getBalances();

	//This compares the new balances with the old ones. 
	std::vector<ChangeOfBalance> changes = newBalances.compareTo(this->currentBalances);

	//If there are any changes to the balance, this will perform the loging specification (every time there is a change of the balance).
	for (ChangeOfBalance& change : changes){
		this->balanceLog.log(change.toCSVString());
	}

	// This will replace old balances with new ones
	this->currentBalances = newBalances;

	//This will return a vector of offers.
	std::vector<Offer> acceptedOffers = this->mm.getAcceptedOffers();
	for (Offer& ao : acceptedOffers) {
		std::cout << "Accepted offer " << ao.toCSVString() << "\n";
		this->tradeLog.log(ao.toCSVString());
	}

	// Get balances, my offers, best offers
	std::vector<Offer> myOffers = this->mm.getMyOffers();
	std::vector<Offer> bestOffers = this->mm.getBestOffers();

	std::uint64_t currentTime = this->mm.getCurrentTime();
	Actions act = this->strategy.cycle(currentTime, myOffers, bestOffers, newBalances);

	for (Offer& o : act.offersToWithdraw) {
		int ret = this->mm.withdrawOffer(o);
		if (ret != 1) {
			std::cout << "WARN: withdrawing offer " << ret << " offers removed";
		}
	}
	for (Offer& o : act.offersToMake) {
		if (this->mm.makeOffer(o)) {
			std::cout << "WARN unable to create offer";
		}
		else {
			this->offerLog.log(o.toCSVString());
		}
	}
}