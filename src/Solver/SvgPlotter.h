
#ifndef SVG_PLOTTER_H
#define SVG_PLOTTER_H

#include <string>
#include <vector>

#include "Airfoil.h"
#include "PanelSolver.h"

/// Lightweight SVG post-processor.
/// Generates a single self-contained SVG file showing:
///   • the airfoil outline
///   • a Cp vs x/c plot (upper surface in blue, lower in red)
///   • annotation: airfoil name, Cl, AoA
class SvgPlotter
{
public:
    SvgPlotter(const Airfoil&    airfoil,
               const Freestream& freestream,
               const PanelSolver& solver);

    /// Write the SVG to `filename`.
    void write(const std::string& filename) const;

private:
    const Airfoil&     airfoil_;
    const Freestream&  freestream_;
    const PanelSolver& solver_;

    // ── Helpers ───────────────────────────────────────────────────────────────

    // Map a world (x/c, y/c) to SVG pixels in the airfoil viewport.
    static void airfoilToSvg(double xoc, double yoc,
                              double ox, double oy, double scale,
                              double& px, double& py);

    // Map (x/c, Cp) to SVG pixels in the Cp chart viewport.
    static void cpToSvg(double xoc, double cp,
                        double ox, double oy,
                        double scaleX, double scaleY,
                        double& px, double& py);
};

#endif // SVG_PLOTTER_H
