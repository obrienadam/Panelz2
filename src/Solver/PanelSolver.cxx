#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <stdexcept>

#include "PanelSolver.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// ─────────────────────────────────────────────────────────────────────────────
// Construction
// ─────────────────────────────────────────────────────────────────────────────

PanelSolver::PanelSolver(const Airfoil& airfoil, const Freestream& freestream)
    :
      airfoil_(airfoil),
      freestream_(freestream)
{
}

// ─────────────────────────────────────────────────────────────────────────────
// Local influence helpers
// ─────────────────────────────────────────────────────────────────────────────

void PanelSolver::sourceVelocityLocal(double xi, double eta, double L,
                                       double& u_s, double& v_s)
{
    // Velocity at (xi, eta) induced by a unit-strength source panel from
    // (0,0) to (L,0) in local panel coordinates.
    //
    // Derivation (Katz & Plotkin §3.14):
    //   u_s = (1/4π) ln[(xi² + eta²) / ((xi-L)² + eta²)]
    //   v_s = (1/2π) [atan2(eta, xi-L) - atan2(eta, xi)]
    //
    // Special case: at the panel mid-point (xi=L/2, eta=0+) the formulae
    // naturally give u_s=0, v_s=1/2 — matching the known self-influence.

    const double eps  = 1.0e-10;
    const double r1sq = xi * xi + eta * eta;
    const double r2sq = (xi - L) * (xi - L) + eta * eta;

    if (r1sq < eps || r2sq < eps)
    {
        // On a panel endpoint — treat as zero to avoid singularity.
        u_s = v_s = 0.0;
        return;
    }

    u_s = (1.0 / (4.0 * M_PI)) * std::log(r1sq / r2sq);
    v_s = (1.0 / (2.0 * M_PI)) * (std::atan2(eta, xi - L) - std::atan2(eta, xi));
}

void PanelSolver::vortexVelocityLocal(double xi, double eta, double L,
                                       double& u_v, double& v_v)
{
    // Vortex panel velocity is the source panel velocity rotated 90°:
    //   u_v = -v_s,   v_v = u_s
    double u_s, v_s;
    sourceVelocityLocal(xi, eta, L, u_s, v_s);
    u_v = -v_s;
    v_v =  u_s;
}

void PanelSolver::localToGlobal(double u_loc, double v_loc, double thetaJ,
                                  double& u_glob, double& v_glob)
{
    const double c = std::cos(thetaJ);
    const double s = std::sin(thetaJ);
    u_glob = c * u_loc - s * v_loc;
    v_glob = s * u_loc + c * v_loc;
}

// ─────────────────────────────────────────────────────────────────────────────
// Combined influence of panel j on control point i
// ─────────────────────────────────────────────────────────────────────────────

void PanelSolver::influence(int i, int j,
                             double& a_ij, double& A_ij,
                             double& c_ij, double& C_ij) const
{
    if (i == j)
    {
        a_ij = 0.5;
        A_ij = 0.0;
        c_ij = 0.0;
        C_ij = 0.5;
        return;
    }

    // Panel j geometry
    const Point3D& startJ = airfoil_.node(j);
    const double   thetaJ = airfoil_.panelAngle(j);
    const double   LJ     = airfoil_.panelLength(j);

    // Control point i
    const Point3D& Ci = airfoil_.panelCenter(i);

    // Transform Ci into panel j's local frame
    const double dx  = Ci.x - startJ.x;
    const double dy  = Ci.y - startJ.y;
    const double cJ  = std::cos(thetaJ);
    const double sJ  = std::sin(thetaJ);
    const double xi  =  cJ * dx + sJ * dy;
    const double eta = -sJ * dx + cJ * dy;

    // Source local velocities
    double u_s_loc, v_s_loc;
    sourceVelocityLocal(xi, eta, LJ, u_s_loc, v_s_loc);

    // Vortex local velocities
    double u_v_loc, v_v_loc;
    vortexVelocityLocal(xi, eta, LJ, u_v_loc, v_v_loc);

    // Rotate back to global frame
    double u_s, v_s, u_v, v_v;
    localToGlobal(u_s_loc, v_s_loc, thetaJ, u_s, v_s);
    localToGlobal(u_v_loc, v_v_loc, thetaJ, u_v, v_v);

    // Panel i normal and tangent (in global frame)
    const Vector3D ni = airfoil_.panelNormal(i);
    const Vector3D ti = airfoil_.panelTangent(i);

    // Project global velocities onto panel-i normal and tangent.
    // Convention matches Katz & Plotkin §3.14: normal component is positive
    // in the direction of panelNormal(i) = (sin θ_i, -cos θ_i).
    a_ij = u_s * ni.x + v_s * ni.y;   // source  → normal    at i
    A_ij = u_s * ti.x + v_s * ti.y;   // source  → tangential at i
    c_ij = u_v * ni.x + v_v * ni.y;   // vortex  → normal    at i
    C_ij = u_v * ti.x + v_v * ti.y;   // vortex  → tangential at i
}

// ─────────────────────────────────────────────────────────────────────────────
// Main solve
// ─────────────────────────────────────────────────────────────────────────────

void PanelSolver::solve()
{
    const int N = airfoil_.numPanels();
    if (N < 2)
        throw std::runtime_error("PanelSolver::solve(): airfoil has fewer than 2 panels.");

    const double Vinf = freestream_.speed();
    if (Vinf < 1.0e-12)
        throw std::runtime_error("PanelSolver::solve(): freestream speed is zero.");

    const Vector3D& V = freestream_.velocity();

    // ── Build (N+1) × (N+1) system ──────────────────────────────────────────
    //
    // Unknowns: x = [σ_0, σ_1, …, σ_{N-1}, γ]   (N source strengths + 1 vortex)
    //
    // Rows 0..N-1: no-penetration (normal velocity = 0 at each control point)
    //   Σ_j  a_{ij} σ_j  +  b_i γ  = -(V · n_i)
    //   where b_i = Σ_j c_{ij}
    //
    // Row N: Kutta condition (smooth flow off trailing edge)
    //   Σ_j (A_{0j} + A_{N-1,j}) σ_j  +  (D_0 + D_{N-1}) γ
    //     = -(V·t_0 + V·t_{N-1})

    DenseMatrix mat = DenseMatrix::Zero(N + 1, N + 1);
    DenseVector rhs = DenseVector::Zero(N + 1);

    // Tangential influence sums for Kutta (D_i = Σ_j C_{ij})
    std::vector<double> D(N, 0.0);

    for (int i = 0; i < N; ++i)
    {
        const Vector3D ni = airfoil_.panelNormal(i);
        double b_i = 0.0;

        for (int j = 0; j < N; ++j)
        {
            double a_ij, A_ij, c_ij, C_ij;
            influence(i, j, a_ij, A_ij, c_ij, C_ij);

            mat(i, j) += a_ij;   // source column j
            b_i       += c_ij;   // vortex column (last)
            D[i]      += C_ij;   // tangential vortex sum for later
        }

        mat(i, N) = b_i;
        rhs(i)    = -(V.x * ni.x + V.y * ni.y);
    }

    // Kutta condition: trailing-edge panels are 0 (upper) and N-1 (lower)
    // in UIUC convention (TE → upper surface → LE → lower surface → TE).
    const int TE_upper = 0;
    const int TE_lower = N - 1;
    const Vector3D t_u = airfoil_.panelTangent(TE_upper);
    const Vector3D t_l = airfoil_.panelTangent(TE_lower);

    // Kutta condition (Katz & Plotkin eq 11.22):
    // Tangential velocity at upper TE + tangential velocity at lower TE = 0
    // (smooth flow off the trailing edge in panel-local frames)
    for (int j = 0; j < N; ++j)
    {
        double a_u, A_u, c_u, C_u;
        double a_l, A_l, c_l, C_l;
        influence(TE_upper, j, a_u, A_u, c_u, C_u);
        influence(TE_lower, j, a_l, A_l, c_l, C_l);
        mat(N, j) = A_u + A_l;
    }
    mat(N, N) = D[TE_upper] + D[TE_lower];
    rhs(N)    = -(V.x * (t_u.x + t_l.x) + V.y * (t_u.y + t_l.y));

    // ── Solve ────────────────────────────────────────────────────────────────

    DenseVector sol = mat.fullPivLu().solve(rhs);

    sigma_.resize(N);
    for (int i = 0; i < N; ++i)
        sigma_[i] = sol(i);

    gamma_ = sol(N);

    // ── Post-process: Vt, Cp, Cl ─────────────────────────────────────────────

    cp_.resize(N);
    Vt_.resize(N);

    double chord = airfoil_.chord();
    if (chord < 1.0e-12) chord = 1.0;

    cl_ = 0.0;

    for (int i = 0; i < N; ++i)
    {
        const Vector3D ti = airfoil_.panelTangent(i);

        // Tangential velocity = freestream tangential + induced from all panels
        double Vt_i = V.x * ti.x + V.y * ti.y;

        for (int j = 0; j < N; ++j)
        {
            double a_ij, A_ij, c_ij, C_ij;
            influence(i, j, a_ij, A_ij, c_ij, C_ij);
            Vt_i += A_ij * sigma_[j];
        }
        Vt_i += D[i] * gamma_;

        Vt_[i]   = Vt_i;
        cp_[i]   = 1.0 - (Vt_i * Vt_i) / (Vinf * Vinf);
    }

    // ── Lift coefficient via Kutta-Joukowski theorem ──────────────────────────
    // L = ρ * Vinf * Γ  where Γ = γ * chord  (total circulation)
    // Cl = L / (0.5 * ρ * Vinf² * chord) = 2 * Γ / (Vinf * chord)
    //    = 2 * γ / Vinf
    // Sign convention: positive γ (CCW from perspective of +z) gives downward
    // force, so for a lifting body we expect γ < 0 (CW = positive lift in +y).
    // Hence: Cl = -2 * gamma_ / Vinf
    double perimeter = 0.0;
    for (int j = 0; j < N; ++j)
        perimeter += airfoil_.panelLength(j);

    cl_ = -2.0 * gamma_ * perimeter / (Vinf * chord);

    solved_ = true;

    std::cout << std::setfill('-') << std::setw(64) << "" << "\n"
              << "Panel solver converged (" << N << " panels)\n"
              << "  Cl = " << std::fixed << std::setprecision(4) << cl_    << "\n"
              << "  γ  = " << gamma_ << "\n"
              << std::setfill('-') << std::setw(64) << "" << "\n";
}


// ─────────────────────────────────────────────────────────────────────────────
// Output
// ─────────────────────────────────────────────────────────────────────────────

void PanelSolver::writeResults(const std::string& filename) const
{
    if (!solved_)
        throw std::runtime_error("PanelSolver::writeResults(): call solve() first.");

    const double chord = airfoil_.chord() > 1.0e-12 ? airfoil_.chord() : 1.0;
    const double Vinf  = freestream_.speed();

    std::ofstream f(filename);
    if (!f.is_open())
        throw std::runtime_error("Cannot open output file: " + filename);

    f << "# Panelz2 results  airfoil=" << airfoil_.name
      << "  Cl=" << cl_ << "  gamma=" << gamma_ << "\n"
      << "x/c,Cp,Vt/Vinf\n";

    f << std::fixed << std::setprecision(6);
    for (int i = 0; i < airfoil_.numPanels(); ++i)
    {
        const double xoc = airfoil_.panelCenter(i).x / chord;
        f << xoc << "," << cp_[i] << "," << (Vt_[i] / Vinf) << "\n";
    }

    std::cout << "Results written to \"" << filename << "\"\n";
}
