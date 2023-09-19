#pragma once

#include <cmath>
#include <ranges>
#include <iostream>
#include <iomanip>
#include <sstream>

using Real = float;

class Vector
{
protected:
    Real values[4];

    Vector(Real x, Real y, Real z, Real p);

public:
    Vector();

    Real& operator[](int index);

    Real operator[](int index) const;
};

class Point : public Vector
{
public:
    Point();

    Point(Real x, Real y, Real z);
};

class Direction : public Vector
{
public:
    Direction();

    Direction(Real x, Real y, Real z);

    // d + p = p
    Point operator+(Point p);

    // d * k = p
    Direction operator*(Real k);

    // d / k = p
    Direction operator/(Real k);
};

Real norm(Direction d);

// Suma de direcciones: d + d = d
Direction operator+(Direction d1, Direction d2);

// Resta de puntos: p - q = d
Direction operator-(Point p, Point q);

// Producto escalar
Real dot(Direction u, Direction v);

// Producto vectorial
Direction cross(Direction u, Direction v);

class Transformation
{
private:
    Real matrix[4][4];

    static void makeIdentity(Real m[4][4]);

    void multiplyFromLeftBy(const Real lMatrix[4][4]);

public:
    Transformation();

    Transformation(Real other[4][4]);

    [[maybe_unused]] Transformation& translate(Direction d);

    [[maybe_unused]] Transformation& scale(Real sx, Real sy, Real sz);

    [[maybe_unused]] Transformation& rotateX(Real alpha);

    [[maybe_unused]] Transformation& rotateY(Real alpha);

    [[maybe_unused]] Transformation& rotateZ(Real alpha);

    [[maybe_unused]] Transformation& apply(const Transformation& t);

    [[maybe_unused]] Transformation& changeBase(Direction u, Direction v, Direction w, Point o);

    [[maybe_unused]] Transformation& revertBase(Direction u, Direction v, Direction w, Point o);

    friend std::ostream& operator<<(std::ostream& os, const Transformation& t);
};

std::ostream& operator<<(std::ostream& os, const Transformation& t);
