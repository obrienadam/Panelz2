// test_airfoil.cpp — Tests for Airfoil panel geometry

#include <cmath>
#include <vector>
#include <gtest/gtest.h>

#include "Airfoil.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

static constexpr double EPS = 1.0e-10;
static constexpr double LOOSE = 1.0e-8;

// ── Helpers ───────────────────────────────────────────────────────────────────

/// Build a horizontal flat plate from (0,0) to (1,0) with `n` equal panels.
static Airfoil makeFlatPlate(int n)
{
    std::vector<Point3D> nodes;
    nodes.reserve(n + 1);
    for (int i = 0; i <= n; ++i)
        nodes.emplace_back(static_cast<double>(i) / n, 0.0);
    Airfoil a;
    a.name = "flat-plate";
    a.init(nodes);
    return a;
}

/// Build a unit square (CCW) with 4 panels.
static Airfoil makeSquare()
{
    // CCW: (0,0) → (1,0) → (1,1) → (0,1) → (0,0)
    std::vector<Point3D> nodes = {
        Point3D(0.0, 0.0), Point3D(1.0, 0.0),
        Point3D(1.0, 1.0), Point3D(0.0, 1.0),
        Point3D(0.0, 0.0)
    };
    Airfoil a;
    a.name = "square";
    a.init(nodes);
    return a;
}

// ── numPanels ─────────────────────────────────────────────────────────────────

TEST(Airfoil, NumPanels)
{
    Airfoil a = makeFlatPlate(5);
    EXPECT_EQ(a.numPanels(), 5);
    EXPECT_EQ(a.numNodes(), 6);
}

// ── panelLength ───────────────────────────────────────────────────────────────

TEST(Airfoil, PanelLengthUniform)
{
    const int N = 10;
    Airfoil a = makeFlatPlate(N);
    for (int i = 0; i < N; ++i)
        EXPECT_NEAR(a.panelLength(i), 1.0 / N, EPS);
}

TEST(Airfoil, PanelLengthSquare)
{
    Airfoil a = makeSquare();
    for (int i = 0; i < a.numPanels(); ++i)
        EXPECT_NEAR(a.panelLength(i), 1.0, EPS);
}

// ── panelAngle ────────────────────────────────────────────────────────────────

TEST(Airfoil, PanelAngleFlatPlateHorizontal)
{
    Airfoil a = makeFlatPlate(4);
    for (int i = 0; i < a.numPanels(); ++i)
        EXPECT_NEAR(a.panelAngle(i), 0.0, EPS);  // all panels along +x
}

TEST(Airfoil, PanelAngleSquare)
{
    Airfoil a = makeSquare();
    // Bottom (+x), right (+y), top (-x), left (-y)
    EXPECT_NEAR(a.panelAngle(0),       0.0,       EPS);
    EXPECT_NEAR(a.panelAngle(1),  M_PI / 2.0, EPS);
    EXPECT_NEAR(std::abs(a.panelAngle(2)), M_PI,       1.0e-10);
    EXPECT_NEAR(std::abs(a.panelAngle(3)),  M_PI / 2.0, EPS);
}

// ── panelTangent ──────────────────────────────────────────────────────────────

TEST(Airfoil, PanelTangentUnitLength)
{
    Airfoil a = makeFlatPlate(6);
    for (int i = 0; i < a.numPanels(); ++i)
    {
        Vector3D t = a.panelTangent(i);
        double mag = std::sqrt(t.x * t.x + t.y * t.y);
        EXPECT_NEAR(mag, 1.0, EPS);
    }
}

TEST(Airfoil, PanelTangentDirection)
{
    Airfoil a = makeFlatPlate(3);
    Vector3D t = a.panelTangent(0);
    EXPECT_NEAR(t.x, 1.0, EPS);
    EXPECT_NEAR(t.y, 0.0, EPS);
}

// ── panelNormal ───────────────────────────────────────────────────────────────

TEST(Airfoil, PanelNormalUnitLength)
{
    Airfoil a = makeSquare();
    for (int i = 0; i < a.numPanels(); ++i)
    {
        Vector3D n = a.panelNormal(i);
        double mag = std::sqrt(n.x * n.x + n.y * n.y);
        EXPECT_NEAR(mag, 1.0, EPS);
    }
}

TEST(Airfoil, PanelNormalOrthogonalToTangent)
{
    Airfoil a = makeSquare();
    for (int i = 0; i < a.numPanels(); ++i)
    {
        Vector3D t = a.panelTangent(i);
        Vector3D n = a.panelNormal(i);
        double dot = t.x * n.x + t.y * n.y;
        EXPECT_NEAR(dot, 0.0, EPS);
    }
}

TEST(Airfoil, PanelNormalFlatPlatePointsDownward)
{
    // Flat plate along +x → tangent = (1,0) → normal = (sin 0, -cos 0) = (0,-1)
    Airfoil a = makeFlatPlate(4);
    for (int i = 0; i < a.numPanels(); ++i)
    {
        Vector3D n = a.panelNormal(i);
        EXPECT_NEAR(n.x, 0.0, EPS);
        EXPECT_NEAR(n.y, -1.0, EPS);
    }
}

// ── panelCenter ───────────────────────────────────────────────────────────────

TEST(Airfoil, PanelCenterMidpoint)
{
    Airfoil a = makeFlatPlate(4);
    // Panel 0: from (0,0) to (0.25,0) → centre (0.125, 0)
    EXPECT_NEAR(a.panelCenter(0).x, 0.125, EPS);
    EXPECT_NEAR(a.panelCenter(0).y, 0.0,   EPS);
}

// ── chord ─────────────────────────────────────────────────────────────────────

TEST(Airfoil, ChordFlatPlate)
{
    Airfoil a = makeFlatPlate(8);
    EXPECT_NEAR(a.chord(), 1.0, EPS);
}

TEST(Airfoil, ChordSquare)
{
    Airfoil a = makeSquare();
    EXPECT_NEAR(a.chord(), 1.0, EPS);  // max_x - min_x = 1
}

// ── init reinitialises cleanly ────────────────────────────────────────────────

TEST(Airfoil, InitReinitialises)
{
    Airfoil a = makeFlatPlate(4);
    EXPECT_EQ(a.numPanels(), 4);

    // Re-init with a different geometry
    std::vector<Point3D> nodes2 = {
        Point3D(0, 0), Point3D(1, 0), Point3D(2, 0)
    };
    a.init(nodes2);
    EXPECT_EQ(a.numPanels(), 2);
}
