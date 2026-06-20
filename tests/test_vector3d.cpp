// test_vector3d.cpp — Unit tests for Vector3D geometry primitives

#include <cmath>
#include <gtest/gtest.h>
#include "Vector3D.h"

static constexpr double EPS = 1.0e-12;

// ── Constructor ───────────────────────────────────────────────────────────────

TEST(Vector3D, DefaultConstruct)
{
    Vector3D v;
    EXPECT_DOUBLE_EQ(v.x, 0.0);
    EXPECT_DOUBLE_EQ(v.y, 0.0);
    EXPECT_DOUBLE_EQ(v.z, 0.0);
}

TEST(Vector3D, Construct)
{
    Vector3D v(1.0, 2.0, 3.0);
    EXPECT_DOUBLE_EQ(v.x, 1.0);
    EXPECT_DOUBLE_EQ(v.y, 2.0);
    EXPECT_DOUBLE_EQ(v.z, 3.0);
}

// ── Magnitude ─────────────────────────────────────────────────────────────────

TEST(Vector3D, Magnitude)
{
    Vector3D v(3.0, 4.0, 0.0);
    EXPECT_NEAR(v.mag(), 5.0, EPS);
}

TEST(Vector3D, MagnitudeUnit)
{
    Vector3D v(1.0, 0.0, 0.0);
    EXPECT_NEAR(v.mag(), 1.0, EPS);
}

// ── Unit vector ───────────────────────────────────────────────────────────────

TEST(Vector3D, UnitVector)
{
    Vector3D v(3.0, 4.0, 0.0);
    Vector3D u = v.unitVector();
    EXPECT_NEAR(u.x, 0.6, EPS);
    EXPECT_NEAR(u.y, 0.8, EPS);
    EXPECT_NEAR(u.z, 0.0, EPS);
    EXPECT_NEAR(u.mag(), 1.0, EPS);
}

// ── Arithmetic operators ──────────────────────────────────────────────────────

TEST(Vector3D, Addition)
{
    Vector3D a(1.0, 2.0, 3.0);
    Vector3D b(4.0, 5.0, 6.0);
    Vector3D c = a + b;
    EXPECT_DOUBLE_EQ(c.x, 5.0);
    EXPECT_DOUBLE_EQ(c.y, 7.0);
    EXPECT_DOUBLE_EQ(c.z, 9.0);
}

TEST(Vector3D, Subtraction)
{
    Vector3D a(4.0, 5.0, 6.0);
    Vector3D b(1.0, 2.0, 3.0);
    Vector3D c = a - b;
    EXPECT_DOUBLE_EQ(c.x, 3.0);
    EXPECT_DOUBLE_EQ(c.y, 3.0);
    EXPECT_DOUBLE_EQ(c.z, 3.0);
}

TEST(Vector3D, ScalarMultiply)
{
    Vector3D v(1.0, 2.0, 3.0);
    Vector3D r = v * 2.0;
    EXPECT_DOUBLE_EQ(r.x, 2.0);
    EXPECT_DOUBLE_EQ(r.y, 4.0);
    EXPECT_DOUBLE_EQ(r.z, 6.0);
}

TEST(Vector3D, ScalarDivide)
{
    Vector3D v(2.0, 4.0, 6.0);
    Vector3D r = v / 2.0;
    EXPECT_NEAR(r.x, 1.0, EPS);
    EXPECT_NEAR(r.y, 2.0, EPS);
    EXPECT_NEAR(r.z, 3.0, EPS);
}

TEST(Vector3D, ScalarMultiplyCommutative)
{
    Vector3D v(1.0, 2.0, 0.0);
    Vector3D r1 = v * 3.0;
    Vector3D r2 = 3.0 * v;
    EXPECT_DOUBLE_EQ(r1.x, r2.x);
    EXPECT_DOUBLE_EQ(r1.y, r2.y);
}

// ── Dot product ───────────────────────────────────────────────────────────────

TEST(Vector3D, DotProductParallel)
{
    Vector3D a(1.0, 0.0, 0.0);
    Vector3D b(1.0, 0.0, 0.0);
    EXPECT_DOUBLE_EQ(dotProduct(a, b), 1.0);
}

TEST(Vector3D, DotProductOrthogonal)
{
    Vector3D a(1.0, 0.0, 0.0);
    Vector3D b(0.0, 1.0, 0.0);
    EXPECT_DOUBLE_EQ(dotProduct(a, b), 0.0);
}

TEST(Vector3D, DotProductGeneral)
{
    Vector3D a(1.0, 2.0, 3.0);
    Vector3D b(4.0, 5.0, 6.0);
    EXPECT_DOUBLE_EQ(dotProduct(a, b), 32.0);  // 4+10+18
}

// ── Cross product ─────────────────────────────────────────────────────────────

TEST(Vector3D, CrossProductXY)
{
    Vector3D a(1.0, 0.0, 0.0);
    Vector3D b(0.0, 1.0, 0.0);
    Vector3D c = crossProduct(a, b);
    EXPECT_NEAR(c.x, 0.0, EPS);
    EXPECT_NEAR(c.y, 0.0, EPS);
    EXPECT_NEAR(c.z, 1.0, EPS);
}

TEST(Vector3D, CrossProductAnticommutative)
{
    Vector3D a(1.0, 2.0, 3.0);
    Vector3D b(4.0, 5.0, 6.0);
    Vector3D ab = crossProduct(a, b);
    Vector3D ba = crossProduct(b, a);
    EXPECT_NEAR(ab.x, -ba.x, EPS);
    EXPECT_NEAR(ab.y, -ba.y, EPS);
    EXPECT_NEAR(ab.z, -ba.z, EPS);
}

// ── Compound assignment ───────────────────────────────────────────────────────

TEST(Vector3D, PlusEquals)
{
    Vector3D v(1.0, 1.0, 1.0);
    v += Vector3D(2.0, 3.0, 4.0);
    EXPECT_DOUBLE_EQ(v.x, 3.0);
    EXPECT_DOUBLE_EQ(v.y, 4.0);
    EXPECT_DOUBLE_EQ(v.z, 5.0);
}

TEST(Vector3D, TimesEquals)
{
    Vector3D v(2.0, 3.0, 4.0);
    v *= 0.5;
    EXPECT_NEAR(v.x, 1.0, EPS);
    EXPECT_NEAR(v.y, 1.5, EPS);
    EXPECT_NEAR(v.z, 2.0, EPS);
}

// ── Cylindrical init ──────────────────────────────────────────────────────────

TEST(Vector3D, InitFromCylindricalXAxis)
{
    Vector3D v;
    v.initFromCylindricalCoords(1.0, 0.0, 0.0);  // angle=0 → +x
    EXPECT_NEAR(v.x, 1.0, EPS);
    EXPECT_NEAR(v.y, 0.0, EPS);
}

TEST(Vector3D, InitFromCylindricalYAxis)
{
    Vector3D v;
    v.initFromCylindricalCoords(1.0, M_PI / 2.0, 0.0);  // angle=90° → +y
    EXPECT_NEAR(v.x, 0.0, 1.0e-10);
    EXPECT_NEAR(v.y, 1.0, 1.0e-10);
}
