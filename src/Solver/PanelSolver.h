#ifndef PANEL_SOLVER_H
#define PANEL_SOLVER_H

#include <string>
#include <vector>

#include "Airfoil.h"
#include "Freestream.h"
#include "Matrix.h"   // Eigen aliases

/// 2-D steady Hess & Smith source/vortex panel method solver.
///
/// Algorithm (constant-strength panels, single global vortex):
///   1. Build (N+1)×(N+1) influence matrix from Katz & Plotkin §3.14.
///   2. Apply no-penetration BCs at each panel control point (rows 0..N-1).
///   3. Enforce the Kutta condition at the trailing edge (row N).
///   4. Solve with Eigen's LU factorisation for σ_i and γ.
///   5. Recover surface tangential velocity and Cp at each panel centre.
///   6. Integrate Cp for the lift coefficient Cl.
class PanelSolver
{
public:
    PanelSolver(const Airfoil& airfoil, const Freestream& freestream);

    /// Run the linear-system solve. Must be called before any accessor.
    void solve();

    // ── Result accessors (valid after solve()) ────────────────────────────────

    /// Lift coefficient Cl = -∫ Cp n_y ds / c
    double cl() const { return cl_; }

    /// Pressure coefficient at each panel centre [size = numPanels]
    const std::vector<double>& cp() const { return cp_; }

    /// Surface tangential speed at each panel centre  [size = numPanels]
    const std::vector<double>& surfaceVelocity() const { return Vt_; }

    /// Source strength σ for each panel  [size = numPanels]
    const std::vector<double>& sourceStrengths() const { return sigma_; }

    /// Global vortex strength γ
    double vortexStrength() const { return gamma_; }

    // ── Output ────────────────────────────────────────────────────────────────

    /// Write a CSV with columns: x/c, Cp, Vt/Vinf for each panel centre.
    void writeResults(const std::string& filename) const;

private:
    const Airfoil&     airfoil_;
    const Freestream&  freestream_;
    bool               solved_ = false;

    // Results
    std::vector<double> sigma_;  // source strengths (N values)
    double              gamma_;  // global vortex strength
    std::vector<double> cp_;     // Cp at each panel centre
    std::vector<double> Vt_;     // |tangential velocity| at each panel centre
    double              cl_;     // lift coefficient

    // ── Influence helpers ────────────────────────────────────────────────────

    /// Velocity (in panel-j's LOCAL coordinate frame) at (xi, eta) induced
    /// by a unit-strength constant-source distribution on a panel of length L.
    ///   Returns {u_s, v_s}  where u_s is along-panel, v_s is normal-to-panel.
    static void sourceVelocityLocal(double xi, double eta, double L,
                                    double& u_s, double& v_s);

    /// Velocity (in panel-j's LOCAL frame) at (xi, eta) induced by a unit-
    /// strength constant-vortex distribution on a panel of length L.
    ///   Returns {u_v, v_v}.  Relation to source: u_v = -v_s, v_v = u_s.
    static void vortexVelocityLocal(double xi, double eta, double L,
                                    double& u_v, double& v_v);

    /// Transform a local-frame velocity (u_loc, v_loc) on panel j (angle θ_j)
    /// back to the global frame.
    static void localToGlobal(double u_loc, double v_loc, double thetaJ,
                               double& u_glob, double& v_glob);

    /// Full influence of panel j on control point i.
    ///   a_ij: normal velocity at panel-i due to unit SOURCE on panel-j
    ///   A_ij: tangential velocity at panel-i due to unit SOURCE on panel-j
    ///   c_ij: normal velocity at panel-i due to unit VORTEX on panel-j
    ///   C_ij: tangential velocity at panel-i due to unit VORTEX on panel-j
    void influence(int i, int j,
                   double& a_ij, double& A_ij,
                   double& c_ij, double& C_ij) const;
};

#endif // PANEL_SOLVER_H
