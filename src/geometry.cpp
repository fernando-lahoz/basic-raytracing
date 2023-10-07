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
Direction operator+(Direction d1, Direction d2)
{
    return {d1[0] + d2[0], d1[1] + d2[1], d1[2] + d2[2]};
}

// Resta de puntos: p - q = d
Direction operator-(Point p, Point q)
{
    return {p[0] - q[0], p[1] - q[1], p[2] - q[2]};
}

// Producto escalar
Real dot(Vector u, Vector v)
{
    return u[0]*v[0] + u[1]*v[1] + u[2]*v[2];
}

// Producto vectorial
Direction cross(Direction u, Direction v)
{
    return {u[1]*v[2] - u[2]*v[1], u[2]*v[0] - u[0]*v[2], u[0]*v[1] - u[1]*v[0]};
}

std::ostream& operator<<(std::ostream& os, const Base& base)
{
    os << "i: " << base.u << '\n';
    os << "j: " << base.v << '\n';
    os << "k: " << base.w << '\n';
    os << "o: " << base.o << '\n';

    return os;
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
            Real sum = 0.0;
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

Point Transformation::operator*(Point p)
{
    auto dotP = [&](int i)
    {
        Real sum = 0.0;
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
        Real sum = 0.0;
        for (auto j : std::views::iota(0, 4))
            sum += matrix[i][j] * d[j];
        return sum;
    };

    return {dotP(0), dotP(1), dotP(2)};
}

Transformation::Transformation(Inverse&& inv)
{
    auto m = [&](int i, int j) { return inv.original.matrix[i][j]; };

    auto cofactor = [&](int u, int v, int w)
    {
        matrix[u][0] = m(1, v) * m(2, w) - m(2, v) * m(1, w);
        matrix[u][1] = m(2, v) * m(0, w) - m(0, v) * m(2, w);
        matrix[u][2] = m(0, v) * m(1, w) - m(1, v) * m(0, w);
    };

    const int U = 0, V = 1, W = 2, O = 3;

    cofactor(U, V, W);
    cofactor(V, W, U);
    cofactor(W, U, V);

    auto rowDotCol = [&](int row, int col)
    {
        return matrix[row][0] * m(0, col) +
               matrix[row][1] * m(1, col) +
               matrix[row][2] * m(2, col);
    };

    Real det = rowDotCol(U, U);

    matrix[U][3] = - rowDotCol(U, O) / det;
    matrix[V][3] = - rowDotCol(V, O) / det;
    matrix[W][3] = - rowDotCol(W, O) / det;

    for (auto j : std::views::iota(0, 3))
    {
        matrix[U][j] /= det;
        matrix[V][j] /= det;
        matrix[W][j] /= det;
        matrix[O][j] = 0.0;
    }  
    matrix[3][3] = 1.0;
}

std::ostream& operator<<(std::ostream& os, const Transformation& t)
{
    for (auto i : std::views::iota(0, 4))
    {
        for (auto j : std::views::iota(0, 4))
        {
            std::stringstream ss;
            //ss << std::setw(10) << std::setprecision(4) << std::fixed << t.matrix[i][j];
            ss << std::setw(15) << std::fixed << t.matrix[i][j];
            os << ss.str();
        }     
        os << '\n';
    }    

    return os;
}

Transformation& Transformation::revertBase(const Base& base)
{
    for (auto j : std::views::iota(0, 4))
    {
        Real column[4];
        for (auto i : std::views::iota(0, 4))
            column[i] = matrix[i][j];

        for (auto i : std::views::iota(0, 4))
        {
            matrix[i][j] =  base.u[i] * column[0] +
                            base.v[i] * column[1] +
                            base.w[i] * column[2] +
                            base.o[i] * column[3];
        }  
    }

    return *this;
}

Transformation& Transformation::changeBase(const Base& base)
{
    auto getCofactorFrom = [](Vector y, Vector z)
    {
        return Direction { y[1] * z[2] - y[2] * z[1],
                           y[2] * z[0] - y[0] * z[2],
                           y[0] * z[1] - y[1] * z[0] };
    };

    auto divide = [](Vector& d, Real k)
        { d[0] /= k; d[1] /= k; d[2] /= k; d[3] /= k; };

    Vector row0 = getCofactorFrom(base.v, base.w);

    Real det = dot(base.u, row0);

    Vector row1 = getCofactorFrom(base.w, base.u);
    Vector row2 = getCofactorFrom(base.u, base.v);
    Point row3 {};

    row0[3] = - dot(base.o, row0);
    row1[3] = - dot(base.o, row1);
    row2[3] = - dot(base.o, row2);

    divide(row0, det);
    divide(row1, det);
    divide(row2, det);

    auto fmuladd = [](Vector v, Vector w)
    {
        Real sum = 0.0;
        for (auto k : std::views::iota(0, 4))
            sum += v[k] * w[k];
        return sum;
    };

    for (auto j : std::views::iota(0, 4))
    {
        Vector column;
        for (auto i : std::views::iota(0, 4))
            column[i] = matrix[i][j];

        matrix[0][j] = fmuladd(row0, column);
        matrix[1][j] = fmuladd(row1, column);
        matrix[2][j] = fmuladd(row2, column);
    }

    return *this;
}