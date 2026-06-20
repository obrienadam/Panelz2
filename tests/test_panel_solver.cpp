// test_panel_solver.cpp — Physics validation tests for PanelSolver
//
// Reference: thin airfoil theory  →  Cl = 2π sin(α)  ≈ 2π α  for small α

#include <cmath>
#include <vector>
#include <algorithm>
#include <gtest/gtest.h>

#include "Airfoil.h"
#include "Freestream.h"
#include "PanelSolver.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// ── Geometry builders ─────────────────────────────────────────────────────────

/// Flat plate: UIUC convention (TE→upper surface→LE→lower→TE), n panels per side.
/// A tiny half-thickness (epsilon) is added so each panel has a well-defined
/// outward normal (a purely co-linear plate gives zero-height panels whose
/// normals are degenerate).  The half-thickness is small enough that lift
/// converges to the thin-airfoil prediction.
/// At α=0 the lift should be ≈ 0.  At angle α, Cl ≈ 2π sin(α).
static Airfoil makeFlatPlateAirfoil(int nPerSide)
{
    // Upper surface: x from 1 → 0 (TE to LE), y = +eps
    // Lower surface: x from 0 → 1 (LE to TE), y = -eps
    const double eps = 1.0e-3;  // tiny half-thickness: 0.1% chord
    std::vector<Point3D> nodes;
    nodes.reserve(2 * nPerSide + 1);

    for (int i = 0; i <= nPerSide; ++i)
    {
        double t = static_cast<double>(i) / nPerSide;
        nodes.emplace_back(1.0 - t, +eps);  // upper surface
    }
    for (int i = 1; i <= nPerSide; ++i)
    {
        double t = static_cast<double>(i) / nPerSide;
        nodes.emplace_back(t, -eps);         // lower surface
    }

    Airfoil a;
    a.name = "flat-plate";
    a.init(nodes);
    return a;
}

/// NACA 4-digit symmetric airfoil with given thickness t (e.g. 0.12 for NACA 0012).
/// n points per surface side.
static Airfoil makeNaca4Symmetric(double thickness, int nPerSide)
{
    // NACA 4-series thickness distribution:
    //   y_t = 5*t*(0.2969√x - 0.1260 x - 0.3516 x² + 0.2843 x³ - 0.1015 x⁴)
    auto yt = [&](double x) -> double {
        return 5.0 * thickness *
               (0.2969 * std::sqrt(x)
                - 0.1260 * x
                - 0.3516 * x * x
                + 0.2843 * x * x * x
                - 0.1015 * x * x * x * x);
    };

    std::vector<Point3D> nodes;
    nodes.reserve(2 * nPerSide + 1);

    // Upper surface TE→LE (cosine spacing for better leading-edge resolution)
    for (int i = 0; i <= nPerSide; ++i)
    {
        double beta = M_PI * static_cast<double>(i) / nPerSide;
        double x    = 0.5 * (1.0 + std::cos(beta));  // 1 → 0
        nodes.emplace_back(x, yt(x));
    }
    // Lower surface LE→TE
    for (int i = 1; i <= nPerSide; ++i)
    {
        double beta = M_PI * (1.0 - static_cast<double>(i) / nPerSide);
        double x    = 0.5 * (1.0 + std::cos(beta));  // 0 → 1
        nodes.emplace_back(x, -yt(x));
    }

    Airfoil a;
    a.name = "NACA-00XX";
    a.init(nodes);
    return a;
}

// ── Tests ─────────────────────────────────────────────────────────────────────

/// At α = 0, a flat plate should give Cl ≈ 0 (within numerical noise).
TEST(PanelSolver, FlatPlateZeroLiftAtZeroAoA)
{
    Airfoil    a = makeFlatPlateAirfoil(30);
    Freestream fs(1.225, 10.0, 0.0);

    PanelSolver solver(a, fs);
    solver.solve();

    EXPECT_NEAR(solver.cl(), 0.0, 0.05);  // within 0.05 of zero
}

/// NACA 0012 at α = 5° → Cl should be within 15% of 2π sin(5°) ≈ 0.548
TEST(PanelSolver, Naca0012LiftAt5deg)
{
    const double alpha    = 5.0 * M_PI / 180.0;
    const double cl_exact = 2.0 * M_PI * std::sin(alpha);  // thin-airfoil reference ≈ 0.548

    Airfoil    a = makeNaca4Symmetric(0.12, 40);
    Freestream fs(1.225, 10.0, alpha);

    PanelSolver solver(a, fs);
    solver.solve();

    // Panel method on NACA 0012 should be within 15% of thin airfoil theory
    EXPECT_NEAR(solver.cl(), cl_exact, std::abs(cl_exact) * 0.15 + 0.05);
}

/// NACA 0012 at α = 10° → Cl should be positive and physically reasonable
TEST(PanelSolver, Naca0012LiftAt10deg)
{
    const double alpha    = 10.0 * M_PI / 180.0;
    const double cl_exact = 2.0 * M_PI * std::sin(alpha);  // thin-airfoil ref ≈ 1.092

    Airfoil    a = makeNaca4Symmetric(0.12, 50);
    Freestream fs(1.225, 10.0, alpha);

    PanelSolver solver(a, fs);
    solver.solve();

    // Panel method should be within 15% of thin airfoil theory
    EXPECT_NEAR(solver.cl(), cl_exact, std::abs(cl_exact) * 0.15 + 0.05);
}

/// Symmetric NACA airfoil at α = 0 should have Cl ≈ 0.
TEST(PanelSolver, SymmetricNaca0012ZeroLiftAtZeroAoA)
{
    Airfoil    a = makeNaca4Symmetric(0.12, 30);
    Freestream fs(1.225, 10.0, 0.0);

    PanelSolver solver(a, fs);
    solver.solve();

    EXPECT_NEAR(solver.cl(), 0.0, 0.05);
}

/// Symmetric airfoil at α = 0: upper and lower surface Cp should be equal
/// (mirrors left-right) because the geometry and flow are symmetric.
TEST(PanelSolver, SymmetricAirfoilCpSymmetry)
{
    Airfoil    a = makeNaca4Symmetric(0.12, 20);
    Freestream fs(1.225, 10.0, 0.0);

    PanelSolver solver(a, fs);
    solver.solve();

    const auto& cp  = solver.cp();
    const int   N   = a.numPanels();
    const double chord = a.chord();

    // For each upper panel, find the closest lower panel by x/c position and
    // check that |Cp_upper - Cp_lower| < threshold.
    for (int i = 0; i < N; ++i)
    {
        const double xoc_i = a.panelCenter(i).x / chord;
        const double ny_i  = a.panelNormal(i).y;

        if (ny_i < 0.0) continue;  // only check upper surface panels

        // Find closest lower panel
        double bestDist = 1.0;
        double cpLower  = 0.0;
        for (int j = 0; j < N; ++j)
        {
            if (a.panelNormal(j).y >= 0.0) continue;
            double dist = std::abs(a.panelCenter(j).x / chord - xoc_i);
            if (dist < bestDist)
            {
                bestDist = dist;
                cpLower  = cp[j];
            }
        }

        if (bestDist < 0.05)  // only compare where we found a close match
            EXPECT_NEAR(cp[i], cpLower, 0.20) << "x/c = " << xoc_i;
    }
}

/// Cp should equal 1 at the stagnation point (front of airfoil, small x/c).
TEST(PanelSolver, CpSaturationNearStagnation)
{
    Airfoil    a = makeNaca4Symmetric(0.12, 30);
    Freestream fs(1.225, 10.0, 0.0);

    PanelSolver solver(a, fs);
    solver.solve();

    const auto& cp    = solver.cp();
    const int   N     = a.numPanels();
    const double chord = a.chord();

    // Find maximum Cp (near stagnation)
    double maxCp = *std::max_element(cp.begin(), cp.end());

    // Cp at stagnation should be close to 1 (within 30% — panel discretization
    // means the stagnation panel only partially captures the full Cp=1 peak).
    EXPECT_NEAR(maxCp, 1.0, 0.30);
}

/// Cp should have suction (Cp < 0) somewhere on the upper surface.
TEST(PanelSolver, UpperSurfaceHasSuction)
{
    const double alpha = 5.0 * M_PI / 180.0;
    Airfoil    a = makeNaca4Symmetric(0.12, 30);
    Freestream fs(1.225, 10.0, alpha);

    PanelSolver solver(a, fs);
    solver.solve();

    const auto& cp    = solver.cp();
    const int   N     = a.numPanels();
    bool        found = false;

    for (int i = 0; i < N; ++i)
        if (a.panelNormal(i).y > 0.0 && cp[i] < -0.05)
            found = true;

    EXPECT_TRUE(found) << "No suction peak found on upper surface at α=5°";
}

/// writeResults should produce a file without throwing.
TEST(PanelSolver, WriteResultsNoThrow)
{
    Airfoil    a = makeNaca4Symmetric(0.12, 20);
    Freestream fs(1.225, 10.0, 5.0 * M_PI / 180.0);

    PanelSolver solver(a, fs);
    solver.solve();

    EXPECT_NO_THROW(solver.writeResults("test_output.csv"));
}
