#include <numbers>
#include <chrono>

#include "geometry.hpp"

void getMatrix(Real matrix[4][4])
{
    for (auto i : std::views::iota(0, 4))
        for (auto j : std::views::iota(0, 4))
            std::cin >> matrix[i][j];
}

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
    //std::cout << "Gimme a matrix pls:\n";

    //Real matrix[4][4];
    //getMatrix(matrix);

    std::cout << "Gimme u, v, w, o:\n";

    Direction u, v, w;
    Point o;

    std::cin >> u[0] >> u[1] >> u[2];
    std::cin >> v[0] >> v[1] >> v[2];
    std::cin >> w[0] >> w[1] >> w[2];
    std::cin >> o[0] >> o[1] >> o[2];

    
    Transformation t {};
    auto seconds = benchmark(1000000000, [&]()
    {
        t.changeBase(u, v, w, o).revertBase(u, v, w, o);
    });

    std::cout << "\nHere you go:\n" << t << '\n';
    std::cout << seconds << " s \n";
    

    //Transformation t {};
    //std::cout << "\nHere you go:\n" << t.revertBase(u, v, w, o) << '\n';
    //std::cout << "\nCheck Identity:\n" << t << '\n';

}