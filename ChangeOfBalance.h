#pragma once
#include <string>

// This class represents a change that happened in the wallet about a particular currency (old vs. new balance). 

class ChangeOfBalance
{
public:
	ChangeOfBalance(std::string _type,
		double _oldBalance,
		double _newBalance);

	std::string toCSVString();

private:
	std::string type;
	double oldBalance;
	double newBalance;
};