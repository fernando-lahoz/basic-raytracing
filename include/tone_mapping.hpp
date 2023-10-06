#pragma once

#include <cmath>
#include <memory>
#include <charconv>

#include "numbers.hpp"
#include "color_spaces.hpp"
#include "image.hpp"

class ToneMappingStrategy
{
public:
    virtual Real operator()(const Image &img, Real v) const = 0;
};

//Capar a partir de 1
class Clamping : public ToneMappingStrategy
{
private:
    Real limit;
public:
    Clamping(Real top) : limit{top} {}

    virtual Real operator()(const Image&, Real v) const override;

    constexpr static std::string_view name = "clamping";
    constexpr static std::string_view alias = "cl";
    constexpr static Index nParams = 1;
};

// Regla de 3 con el valor más alto
class Equalization : public ToneMappingStrategy
{
public:
    virtual Real operator()(const Image& img, Real v) const override;
    constexpr static std::string_view name = "equalization";
    constexpr static std::string_view alias = "eq";
    constexpr static Natural nParams = 0;
};

// Elegir un valor como el máximo
class Equalization_Clamping : public ToneMappingStrategy
{
private:
    Real limit;
public:
    Equalization_Clamping(Real top) : limit{top} {}

    virtual Real operator()(const Image&, Real v) const override;

    constexpr static std::string_view name = "equalization_clamping";
    constexpr static std::string_view alias = "eq_cl";
    constexpr static Natural nParams = 1;
};

class Gamma : public ToneMappingStrategy
{
private:
    Real gammaValue;
public:
    Gamma(Real gamma) : gammaValue{gamma} {}

    virtual Real operator()(const Image& img, Real v) const override;

    constexpr static std::string_view name = "gamma";
    constexpr static std::string_view alias = "gm";
    constexpr static Natural nParams = 1;
};

class Gamma_Clamping : public ToneMappingStrategy
{
private:
    Real limit;
    Real gammaValue;
public:
    Gamma_Clamping(Real top, Real gamma) : limit{top}, gammaValue{gamma} {}
    
    virtual Real operator()(const Image&, Real v) const override;

    constexpr static std::string_view name = "gamma_clamping";
    constexpr static std::string_view alias = "gm_cl";
    constexpr static Natural nParams = 2;
};

[[nodiscard]] std::unique_ptr<ToneMappingStrategy> makeToneMappingStrategy(std::string_view strategy);
