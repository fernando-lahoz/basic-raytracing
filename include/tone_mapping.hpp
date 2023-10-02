#pragma once

#include <cmath>

#include "numbers.hpp"
#include "color_spaces.hpp"

class ToneMappingStrategy
{
public:
    virtual Real operator()(Real luminance) const = 0;
};

//Capar a partir de 1
class Clamping : public ToneMappingStrategy
{
public:
    virtual Real operator()(Real luminance) const override
    {
        return numbers::min(luminance, 1);
    }
};

// Regla de 3 con el valor más alto
class Equalization : public ToneMappingStrategy
{
private:
    Real maxLuminance;
public:
    Equalization(Real max) : maxLuminance{max} {}

    virtual Real operator()(Real luminance) const override
    {
        return luminance / maxLuminance;
    }
};

// Elegir un valor como el máximo
class Equalization_Clamping : public ToneMappingStrategy
{
private:
    Real limit;
public:
    Equalization_Clamping(Real top) : limit{top} {}
    virtual Real operator()(Real luminance) const override
    {
        return numbers::min(luminance, limit) / limit;
    }
};

class Gamma : public ToneMappingStrategy
{
private:
    Real maxLuminance;
    Real gammaValue;
public:
    Gamma(Real max, Real gamma) : maxLuminance{max}, gammaValue{gamma} {}
    virtual Real operator()(Real luminance) const override
    {
        return std::pow(luminance / maxLuminance , gammaValue);
    }
};

class Gamma_Clamping : public ToneMappingStrategy
{
private:
    Real limit;
    Real gammaValue;
public:
    Gamma_Clamping(Real top, Real gamma) : limit{top}, gammaValue{gamma} {}
    virtual Real operator()(Real luminance) const override
    {
        return std::pow(std::min(luminance, limit) / limit , gammaValue);
    }
};

