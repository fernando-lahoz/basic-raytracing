#include "geometry.hpp"

std::istream& operator>>(std::istream& is, Vector& v)
{
    is >> v.values[0] >> v.values[1] >> v.values[2];
    return is;
}

std::ostream& operator<<(std::ostream& os, Vector v)
{
    os << '(' << v.values[0] << ", " << v.values[1] << ", " << v.values[2] << ')';
    return os;
}

Point operator+(Direction d, Point p)
{
    return {d[0] + p[0], d[1] + p[1], d[2] + p[2]};
}

Point operator+(Point p, Direction d)
{
    return d + p;
}

// d * k = p
Direction operator*(Real k, Direction d)
{
    return {d[0] * k, d[1] * k, d[2] * k};
}

Direction operator*(Direction d, Real k)
{
    return k * d;
}

// d / k = p
Direction operator/(Direction d, Real k)
{
    return {d[0] / k, d[1] / k, d[2] / k};
}

Real norm(Direction d)
{
    return std::sqrt(d[0] * d[0] + d[1] * d[1] + d[2] * d[2]);
}

Direction normalize(Direction d)
{
    return d / norm(d);
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
    return u[0]*v[0] + u[1]*v[1] + u[2]*v[2];
}

// Producto vectorial
Direction cross(Direction u, Direction v)
{
    return {u[1]*v[2] - u[2]*v[1], u[2]*v[0] - u[0]*v[2], u[0]*v[1] - u[1]*v[0]};
}

Base::Base(Direction u, Direction v, Direction w, Point o)
{
    auto getCofactorFrom = [](Direction y, Direction z)
    {
        return Direction { y[1] * z[2] - y[2] * z[1],
                        y[2] * z[0] - y[0] * z[2],
                        y[0] * z[1] - y[1] * z[0] };
    };

    auto dotP = [](Point p, Direction d) { return p[0]*d[0] + p[1]*d[1] + p[2]*d[2]; };

    Direction uI = getCofactorFrom(v, w);

    Real det = dot(u, uI);

    if (det != 0)
    {
        invertible = true;

        Direction vI = getCofactorFrom(w, u);
        Direction wI = getCofactorFrom(u, v);

        uI[3] = - dotP(o, uI);
        vI[3] = - dotP(o, vI);
        wI[3] = - dotP(o, wI);

        for (auto j : std::views::iota(0, 4))
        {
            revertMatrix[0][j] = uI[j] / det;
            revertMatrix[1][j] = vI[j] / det;
            revertMatrix[2][j] = wI[j] / det;
        }
        revertMatrix[3][3] = 1.0f;

        for (auto i : std::views::iota(0, 4))
        {
            changeMatrix[i][0] = u[i];
            changeMatrix[i][1] = v[i];
            changeMatrix[i][2] = w[i];
            changeMatrix[i][3] = o[i];
        }
    }
    else
    {
        invertible = false;
    }
}

void Transformation::makeIdentity(Real m[4][4])
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

Transformation& Transformation::changeBase(const Base& base)
{
    multiplyFromLeftBy(base.changeMatrix);
    return *this;
}

// Inversa por adjuntos
Transformation& Transformation::revertBase(const Base& base)
{
    multiplyFromLeftBy(base.revertMatrix);

    return *this;
}

Point Transformation::operator*(Point p)
{
    auto dotP = [&](int i)
    {
        Real sum = 0.0f;
        for (auto j : std::views::iota(0, 4))
            sum += matrix[i][j] * p[j];
        return sum;
    };

    return {dotP(0), dotP(1), dotP(2)};
}

Direction Transformation::operator*(Direction d)
{
    auto dotP = [&](int i)
    {
        Real sum = 0.0f;
        for (auto j : std::views::iota(0, 4))
            sum += matrix[i][j] * d[j];
        return sum;
    };

    return {dotP(0), dotP(1), dotP(2)};
}

std::ostream& operator<<(std::ostream& os, const Transformation& t)
{
    for (auto i : std::views::iota(0, 4))
    {
        for (auto j : std::views::iota(0, 4))
        {
            std::stringstream ss;
            ss << std::setw(10) << std::setprecision(4) << std::fixed << t.matrix[i][j];
            os << ss.str();
        }     
        os << '\n';
    }    

    return os;
}