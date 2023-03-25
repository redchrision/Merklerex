#include <sstream>
#include "ChangeOfBalance.h"

ChangeOfBalance::ChangeOfBalance(std::string _type, double _oldBalance, double _newBalance)
    : type(_type),
    oldBalance(_oldBalance),
    newBalance(_newBalance)
{
}

// This logs outputs ChangeOfBalance as a CSV formated string, for logging purposes. 

std::string ChangeOfBalance::toCSVString()
{
    std::ostringstream stringStream;
    stringStream << this->type << "," << this->oldBalance << "," << this->newBalance;
    std::string copyOfStr = stringStream.str();
    return copyOfStr;
}