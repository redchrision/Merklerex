#include "Offer.h"
#include <sstream>

Offer::Offer(double _amountOffered, std::string _typeOffered, double _amountWanted, std::string _typeWanted)
    : amountOffered(_amountOffered),
    typeOffered(_typeOffered),
    amountWanted(_amountWanted),
    typeWanted(_typeWanted)
{

}

std::string Offer::toCSVString()
{
    std::ostringstream stringStream;
    stringStream << this->typeWanted << "," << this->typeOffered << "," << this->amountWanted << "," << this->amountOffered;
    std::string copyOfStr = stringStream.str();
    return copyOfStr;
}

std::string Offer::CSVHeader()
{
    return "Type Wanted, Type Offered, Amount Wanted, Amount Offered";
}
