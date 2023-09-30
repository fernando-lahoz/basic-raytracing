#pragma once

#include <fstream>

//10^-6, 10^8 -> 0, 255

enum class strategy
{
    clamping,
    equalization,
    equalization_clamping,
    gamma,
    gamma_clamping
};

namespace strategy_implementation
{
    //Capar a partir de un valor a 255
    auto clamping()
    {

    }

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

} // namespace strategy


