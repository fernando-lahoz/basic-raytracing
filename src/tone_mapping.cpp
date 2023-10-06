#include "tone_mapping.hpp"


Real Clamping::operator()(const Image&, Real v) const
{
    return numbers::min(v, 1);
}

Real Equalization::operator()(const Image& img, Real v) const
{
    return v / img.luminance();
}

Real Equalization_Clamping::operator()(const Image&, Real v) const
{
    return numbers::min(v, limit) / limit;
}

Real Gamma::operator()(const Image& img, Real v) const
{
    return std::pow(v / img.luminance() , gammaValue);
}

Real Gamma_Clamping::operator()(const Image&, Real v) const
{
    return std::pow(std::min(v, limit) / limit , gammaValue);
}


template <typename TM>
bool checkName (std::string_view name)
{ 
    return name == TM::name || name == TM::alias;
};

template <typename TM>
std::unique_ptr<TM> checkStrategy(Index dots,std::string_view strategy)
{
    Real params[10];
    for (Index i = 0; i < TM::nParams; i++)
    {
        if (dots == std::string_view::npos)
            return nullptr;

        auto predots = dots;
        dots = strategy.find_first_of(':', dots + 1);
        auto num = strategy.substr(predots + 1, dots - (predots + 1));
        if (num.empty())
            return nullptr;

        const char* begin = num.data(), *end = num.data() + num.length();
        auto res = std::from_chars(begin, end, params[i]);
        if (res.ec != std::errc{})
            return nullptr;
    }

    if constexpr (TM::nParams == 0) return std::make_unique<TM>();
    else if constexpr (TM::nParams == 1) return std::make_unique<TM>(params[0]);
    else if constexpr (TM::nParams == 2) return std::make_unique<TM>(params[0], params[1]);
    else return nullptr;
};

std::unique_ptr<ToneMappingStrategy> makeToneMappingStrategy(std::string_view strategy)
{
    Index dots = strategy.find_first_of(':');
    const auto name = strategy.substr(0, dots);

    if (checkName<Clamping>(name)) return checkStrategy<Clamping>(dots, strategy);
    else if (checkName<Equalization>(name)) return checkStrategy<Equalization>(dots, strategy);
    else if (checkName<Equalization_Clamping>(name)) return checkStrategy<Equalization_Clamping>(dots, strategy);
    else if (checkName<Gamma>(name)) return checkStrategy<Gamma>(dots, strategy);
    else if (checkName<Gamma_Clamping>(name)) return checkStrategy<Gamma_Clamping>(dots, strategy);
    return nullptr;
}
