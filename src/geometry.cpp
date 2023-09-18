#include "geometry.hpp"

Vector::Vector(Real x, Real y, Real z, Real p)
    : values{x, y, z, p}
{}

Vector::Vector()
    : values{}
{}

Real& Vector::operator[](int index)
{
    return values[index];
}

Real Vector::operator[](int index) const
{
    return values[index];
}

Point::Point()
    : Vector{}
{}

Point::Point(Real x, Real y, Real z)
    : Vector{x, y, z, 1}
{}    

Direction::Direction()
    : Vector{}
{}

Direction::Direction(Real x, Real y, Real z)
    : Vector{x, y, z, 0}
{}

// d + p = p
Point Direction::operator+(Point p) {
    Direction &d = *this;
    return {d[0] + p[0], d[1] + p[1], d[2] + p[2]};
}

// d + k = p
Point Direction::operator*(Real k) {
    Direction &d = *this;
    return {d[0] * k, d[1] * k, d[2] * k};
}

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

void Transformation::multiplyFromLeftBy(const Real lMatrix[4][4])
{
    for (auto j : std::views::iota(0, 4))
    {
        Real column[4];
        for (auto i : std::views::iota(0, 4))
            column[i] = matrix[i][j];

        for (auto i : std::views::iota(0, 4))
        {
            Real sum = 0.0f;
            for (auto k : std::views::iota(0, 4))
                sum += lMatrix[i][k] * column[k];

            matrix[i][j] = sum;
        }   
    }
}

Transformation::Transformation()
{
    makeIdentity(matrix);
}

Transformation::Transformation(Real other[4][4])
{
    for (auto i : std::views::iota(0, 4))
        for (auto j : std::views::iota(0, 4))
            matrix[i][j] = other[i][j];
}

Transformation& Transformation::translate(Direction d)
{
    matrix[0][3] += d[0];
    matrix[1][3] += d[1];
    matrix[2][3] += d[2]; 

    return *this;
}

Transformation& Transformation::scale(Real sx, Real sy, Real sz)
{
    for (auto j : std::views::iota(0, 4))
    {
        matrix[0][j] *= sx;
        matrix[1][j] *= sy;
        matrix[2][j] *= sz;
    }

    return *this;
}

Transformation& Transformation::rotateX(Real alpha)
{
    for (auto j : std::views::iota(0, 4))
    {
        Real m1j = matrix[1][j];
        Real m2j = matrix[2][j];

        matrix[1][j] = std::cos(alpha) * m1j - std::sin(alpha) * m2j;
        matrix[2][j] = std::sin(alpha) * m1j + std::cos(alpha) * m2j;
    }

    return *this;
}

Transformation& Transformation::rotateY(Real alpha)
{
    for (auto j : std::views::iota(0, 4))
    {
        Real m0j = matrix[0][j];
        Real m2j = matrix[2][j];

        matrix[0][j] = std::cos(alpha) * m0j + std::sin(alpha) * m2j;
        matrix[2][j] = -std::sin(alpha) * m0j + std::cos(alpha) * m2j;
    }

    return *this;
    }

Transformation& Transformation::rotateZ(Real alpha)
{
    for (auto j : std::views::iota(0, 4))
    {
        Real m0j = matrix[0][j];
        Real m1j = matrix[1][j];

        matrix[0][j] = std::cos(alpha) * m0j - std::sin(alpha) * m1j;
        matrix[1][j] = std::sin(alpha) * m0j + std::cos(alpha) * m1j;
    }

    return *this;
}

Transformation& Transformation::apply(const Transformation& t)
{
    multiplyFromLeftBy(t.matrix);
    return *this;
}

Transformation& Transformation::changeBase(Direction u, Direction v, Direction w, Point o)
{
    Real base[4][4];
    for (auto i : std::views::iota(0, 4))
    {
        base[i][0] = u[i];
        base[i][1] = v[i];
        base[i][2] = w[i];
        base[i][3] = o[i];
    }

    multiplyFromLeftBy(base);
    return *this;
}

std::ostream& operator<<(std::ostream& os, const Transformation& t)
{
    for (auto i : std::views::iota(0, 4))
    {
        for (auto j : std::views::iota(0, 4))
            os << t.matrix[i][j] << '\t';
        os << '\n';
    }    

    return os;
}