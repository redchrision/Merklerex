#pragma once
#include <string>

// This class represents an offer to trade one currency for another. 

class Offer
{
public:
	Offer(double _amountOffered,
		std::string _typeOffered,
		double amountWanted,
		std::string _typeWanted);
	
	std::string toCSVString();

	static std::string CSVHeader();

	double amountOffered;
	std::string typeOffered;
	double amountWanted;
	std::string typeWanted;
};