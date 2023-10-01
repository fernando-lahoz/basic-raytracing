#pragma once

#include "numbers.hpp"
#include "color_spaces.hpp"

class ToneMappingStrategy
{
public:
    virtual Real operator()(Real luminance) const = 0;
};

//Capar a partir de 255
class Clamping : public ToneMappingStrategy
{
public:
    virtual Real operator()(Real luminance) const override
    {
        return numbers::min(luminance, 255);
    }
};

// Regla de 3 con el valor más alto
class equalization : public ToneMappingStrategy
{
public:
    virtual Real operator()(Real luminance) const override
    {
        return luminance;
    }
};

// Elegir un valor como el máximo
class equalization_clamping : public ToneMappingStrategy
{
public:
    virtual Real operator()(Real luminance) const override
    {
        return luminance;
    }
};

// exp(1/n)
class gamma : public ToneMappingStrategy
{
public:
    virtual Real operator()(Real luminance) const override
    {
        return luminance;
    }
};

class gamma_clamping : public ToneMappingStrategy
{
public:
    virtual Real operator()(Real luminance) const override
    {
        return luminance;
    }
};

