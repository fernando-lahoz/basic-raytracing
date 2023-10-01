#pragma once

#include <cmath>
#include <ranges>
#include <istream>
#include <ostream>
#include <iomanip>
#include <sstream>
#include <optional>

#include "numbers.hpp"
#include "macros/constructor_wrapper.hpp"

class Vector
{
protected:
    Real values[4];

    Vector(Real x, Real y, Real z, Real p) : values{x, y, z, p} {}

public:
    Vector() : values{} {};

    [[nodiscard]] Real& operator[](int index) { return values[index]; }

    [[nodiscard]] Real operator[](int index) const { return values[index]; };

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

[[nodiscard]] Real norm(Direction d);

[[nodiscard]] Direction normalize(Direction d);

// d * k = p
[[nodiscard]] Direction operator*(Real k, Direction d);
[[nodiscard]] Direction operator*(Direction d, Real k);

// d / k = p
[[nodiscard]] Direction operator/(Direction d, Real k);

// d + p = p
[[nodiscard]] Point operator+(Direction d, Point p);
[[nodiscard]] Point operator+(Point p, Direction d);

// Suma de direcciones: d + d = d
[[nodiscard]] Direction operator+(Direction d1, Direction d2);

// Resta de puntos: p - q = d
[[nodiscard]] Direction operator-(Point p, Point q);

// Producto escalar
[[nodiscard]] Real dot(Direction u, Direction v);

// Producto vectorial
[[nodiscard]] Direction cross(Direction u, Direction v);

struct Base
{
    Direction u, v, w;
    Point o;
    
    friend std::ostream& operator<<(std::ostream& os, const Base& base);
};

std::ostream& operator<<(std::ostream& os, const Base& base);

class Transformation
{
private:
    Real matrix[4][4];

    static void makeIdentity(Real m[4][4]);

    void multiplyFromLeftBy(const Real lMatrix[4][4]);

    Transformation(Real other[4][4]);

public:
    Transformation() { makeIdentity(matrix); }

    [[maybe_unused]] Transformation& translate(Direction d);

    [[maybe_unused]] Transformation& scale(Real sx, Real sy, Real sz);

    [[maybe_unused]] Transformation& rotateX(Real alpha);

    [[maybe_unused]] Transformation& rotateY(Real alpha);

    [[maybe_unused]] Transformation& rotateZ(Real alpha);

    [[maybe_unused]] Transformation& apply(const Transformation& t);

    [[maybe_unused]] Transformation& revertBase(const Base& base);

    [[maybe_unused]] Transformation& changeBase(const Base& base);

    [[nodiscard]] Point operator*(Point p);
    [[nodiscard]] Direction operator*(Direction d);

    CONSTRUCTOR_WRAPPER(Transformation, Inverse)

    friend std::ostream& operator<<(std::ostream& os, const Transformation& t);
};

std::ostream& operator<<(std::ostream& os, const Transformation& t);
