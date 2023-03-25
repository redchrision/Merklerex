#pragma once
#include "Offer.h"
#include <vector>

class OffersAtTime
{
public:
    OffersAtTime(std::uint64_t _time, std::vector<Offer> _offers): time(_time), offers(_offers) {}
    std::uint64_t time;
    std::vector<Offer> offers;
};