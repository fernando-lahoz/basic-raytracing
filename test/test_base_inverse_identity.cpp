#include <numbers>
#include <chrono>
#include <iostream>

#include "geometry.hpp"

double benchmark(int64_t times, auto lambda)
{
    auto start = std::chrono::system_clock::now();

    for ([[maybe_unused]] int64_t x = 0; x < times; x++)
    {
        lambda();
    }

    auto end = std::chrono::system_clock::now();
    return std::chrono::duration_cast<std::chrono::duration<double>>(end - start).count();
}

const Real PI = std::numbers::pi_v<Real>;

int main()
{
    std::cout << "Gimme u, v, w, o:\n";

    Direction u, v, w;
    Point o;

    std::cin >> u[0] >> u[1] >> u[2];
    std::cin >> v[0] >> v[1] >> v[2];
    std::cin >> w[0] >> w[1] >> w[2];
    std::cin >> o[0] >> o[1] >> o[2];

    
    Transformation t, rb, cb;
    Base b {u, v, w, o};

    //t.revertBase(b);
    t.revertBase(b);
    rb.revertBase(b);
    cb = Transformation::Inverse(rb);
    std::cout << "\nBase:\n" << rb << '\n';
    std::cout << "\nInverse:\n" << cb << '\n';

/*
    auto seconds = benchmark(1000000000, [&]()
    {
        t.changeBase(b).revertBase(b);
    });

    
*/
    auto seconds = benchmark(1000000000, [&]()
    {
        t.changeBase(b).revertBase(b);
        //t.apply(cb).apply(rb);
    });

    std::cout << "\nHere you go:\n" << t << '\n';
    std::cout << seconds << " s \n";
}