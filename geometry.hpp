#include <cmath>
#include <ranges>

using Real = float;

class Vector
{
protected:
    Real values[4];

    Vector(Real x, Real y, Real z, Real p)
        : values{x, y, z, p}
    {}

public:
    Vector()
        : values{}
    {}

    Real& operator[](int index)
    {
        return values[index];
    }

    Real operator[](int index) const
    {
        return values[index];
    }
};

class Point : public Vector
{
public:
    Point()
        : Vector{}
    {}

    Point(Real x, Real y, Real z)
        : Vector{x, y, z, 1}
    {}    
};

class Direction : public Vector
{
public:
    Direction()
        : Vector{}
    {}

    Direction(Real x, Real y, Real z)
        : Vector{x, y, z, 0}
    {}

    // d + p = p
    Point operator+(Point p) {
        Direction &d = *this;
        return {d[0] + p[0], d[1] + p[1], d[2] + p[2]};
    }

    // d + k = p
    Point operator*(Real k) {
        Direction &d = *this;
        return {d[0] * k, d[1] * k, d[2] * k};
    }
};

Real norm(Direction d)
{
    return std::sqrt(d[0] * d[0] + d[1] * d[1] + d[2] * d[2]);
}

// Suma de direcciones: d + d = d
Direction operator+(Direction d1, Direction d2) {
    return {d1[0] + d2[0], d1[1] + d2[1], d1[2] + d2[2]};
}

// Resta de puntos: p - q = d
Direction operator-(Point p, Point q) {
    return {p[0] - q[0], p[1] - q[1], p[2] - q[2]};
}

// Producto escalar
Real dot(Direction u, Direction v)
{
    return u[0]*v[0] + u[0]*v[0] + u[2]*v[2];
}

// Producto vectorial
Direction cross(Direction u, Direction v)
{
    return {u[1]*v[2] - u[2]*v[1], u[2]*v[0] - u[0]*v[2], u[0]*v[1] - u[1]*v[0]};
}

void makeIdentity(Real m[4][4])
{
    for (auto i : std::views::iota(0, 4))
        for (auto j : std::views::iota(0, 4))
            if (i == j)
                m[i][j] = 1;
            else
                m[i][j] = 0;
}

class Transformation
{
private:
    Real matrix[4][4];

public:

    Transformation()
    {
        makeIdentity(matrix);
    }

    [[maybe_unused]] Transformation& translate(Direction t)
    {
        matrix[0][3] += t[0];
        matrix[1][3] += t[1];
        matrix[2][3] += t[2]; 

        return *this;
    }

    [[maybe_unused]] Transformation& rotateX(Real alpha)
    {
        return *this;
    }

};