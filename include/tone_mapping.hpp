#pragma once

#include "real_numbers.hpp"

//10^-6, 10^8 -> 0, 255

class ToneMappingStrategy
{
public:
    virtual Real operator()(Real v) const = 0;
};

//Capar a partir de un valor a 255
class Clamping : public ToneMappingStrategy
{
public:
    virtual Real operator()(Real v) const override
    {

    }
};

// Regla de 3 con el valor más alto
auto equalization()
{

}

// Elegir un valor como el máximo
auto equalization_clamping()
{

}

// exp(1/n)
auto gamma()
{

}

auto gamma_clamping()
{

}

