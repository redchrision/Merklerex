#pragma once

#include <vector>
#include "Offer.h"

class Actions
{
public:
	/// <summary>
	///  List of offers which should be withdrawn to free up funds
	/// </summary>
	std::vector<Offer> offersToWithdraw;

	/// <summary>
	/// List of offers which should be created after offersToWithdraw have been withdrawn
	/// </summary>
	std::vector<Offer> offersToMake;
};

