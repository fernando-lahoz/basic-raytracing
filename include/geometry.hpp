#pragma once

#include <cmath>
#include <ranges>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <optional>

using Real = float;

class Vector
{
protected:
    Real values[4];

    Vector(Real x, Real y, Real z, Real p) : values{x, y, z, p} {}

public:
    Vector() : values{} {};

    Real& operator[](int index) { return values[index]; }

    Real operator[](int index) const { return values[index]; };

    friend std::istream& operator>>(std::istream& is, Vector& v);
    friend std::ostream& operator<<(std::ostream& os, Vector v);
};

std::istream& operator>>(std::istream& is, Vector& v);
std::ostream& operator<<(std::ostream& os, Vector v);

class Point : public Vector
{
public:
    Point() : Vector{0, 0, 0, 1} {}

    Point(Real x, Real y, Real z) : Vector{x, y, z, 1} {}
};

class Direction : public Vector
{
public:
    Direction() : Vector{0, 0, 0, 0} {}

    Direction(Real x, Real y, Real z) : Vector{x, y, z, 0} {}
};

Real norm(Direction d);

Direction normalize(Direction d);

// d * k = p
Direction operator*(Real k, Direction d);
Direction operator*(Direction d, Real k);

// d / k = p
Direction operator/(Direction d, Real k);

// d + p = p
Point operator+(Direction d, Point p);
Point operator+(Point p, Direction d);

// Suma de direcciones: d + d = d
Direction operator+(Direction d1, Direction d2);

// Resta de puntos: p - q = d
Direction operator-(Point p, Point q);

// Producto escalar
Real dot(Direction u, Direction v);

// Producto vectorial
Direction cross(Direction u, Direction v);

class Base
{
private:
    Real changeMatrix[4][4];
    Real revertMatrix[4][4];
    bool invertible;
    
public:
    Base() {}

    Base(Direction u, Direction v, Direction w, Point o);

    bool isBase() { return invertible; }

    friend class Transformation;
};

class Transformation
{
private:
    Real matrix[4][4];

    static void makeIdentity(Real m[4][4]);

    void multiplyFromLeftBy(const Real lMatrix[4][4]);

public:
    Transformation() { makeIdentity(matrix); }

    Transformation(Real other[4][4]);

    [[maybe_unused]] Transformation& translate(Direction d);

    [[maybe_unused]] Transformation& scale(Real sx, Real sy, Real sz);

    [[maybe_unused]] Transformation& rotateX(Real alpha);

    [[maybe_unused]] Transformation& rotateY(Real alpha);

    [[maybe_unused]] Transformation& rotateZ(Real alpha);

    [[maybe_unused]] Transformation& apply(const Transformation& t);

    [[maybe_unused]] Transformation& changeBase(const Base& base);

    [[maybe_unused]] Transformation& revertBase(const Base& base);

    Point operator*(Point p);
    Direction operator*(Direction d);

    friend std::ostream& operator<<(std::ostream& os, const Transformation& t);
};

std::ostream& operator<<(std::ostream& os, const Transformation& t);
