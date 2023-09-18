#include <numbers>
#include <chrono>

#include "geometry.hpp"

void getMatrix(Real matrix[4][4])
{
    for (auto i : std::views::iota(0, 4))
        for (auto j : std::views::iota(0, 4))
            std::cin >> matrix[i][j];
}

const Real PI = std::numbers::pi_v<Real>;

int main()
{
    std::cout << "Gimme a matrix pls:\n";

    Real matrix[4][4];
    getMatrix(matrix);

    std::cout << "Gimme u, v, w, o:\n";

    Direction u, v, w;
    Point o;

    std::cin >> u[0] >> u[1] >> u[2] >> u[3];
    std::cin >> v[0] >> v[1] >> v[2] >> v[3];
    std::cin >> w[0] >> w[1] >> w[2] >> w[3];
    std::cin >> o[0] >> o[1] >> o[2] >> o[3];

    auto start = std::chrono::system_clock::now();

    Transformation t {matrix};
    //9223372036854775807
    //1000000000000000
    for ([[maybe_unused]] int64_t x = 0; x < 1000000000; x++)
    {
        t.changeBase(u, v, w, o);
    }

    auto end = std::chrono::system_clock::now();
    auto seconds = std::chrono::duration_cast<std::chrono::duration<double>>(end - start).count();
    
    std::cout << "\nHere you go:\n" << t << '\n';

    std::cout << seconds << " s \n";

}