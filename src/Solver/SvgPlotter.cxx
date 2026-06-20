#include <algorithm>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <stdexcept>

#include "SvgPlotter.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// ─────────────────────────────────────────────────────────────────────────────
// Construction
// ─────────────────────────────────────────────────────────────────────────────

SvgPlotter::SvgPlotter(const Airfoil&     airfoil,
                       const Freestream&  freestream,
                       const PanelSolver& solver)
    :
      airfoil_(airfoil),
      freestream_(freestream),
      solver_(solver)
{
}

// ─────────────────────────────────────────────────────────────────────────────
// Coordinate helpers
// ─────────────────────────────────────────────────────────────────────────────

void SvgPlotter::airfoilToSvg(double xoc, double yoc,
                               double ox, double oy, double scale,
                               double& px, double& py)
{
    // SVG y-axis is flipped
    px = ox + xoc * scale;
    py = oy - yoc * scale;
}

void SvgPlotter::cpToSvg(double xoc, double cp,
                          double ox, double oy,
                          double scaleX, double scaleY,
                          double& px, double& py)
{
    px = ox + xoc * scaleX;
    py = oy + cp  * scaleY;   // Cp positive downward (convention: suction on top)
}

// ─────────────────────────────────────────────────────────────────────────────
// SVG writer
// ─────────────────────────────────────────────────────────────────────────────

void SvgPlotter::write(const std::string& filename) const
{
    // ── Layout constants (pixels) ────────────────────────────────────────────
    const int totalW  = 900;
    const int totalH  = 700;

    // Airfoil panel (top half)
    const double afOX    = 80.0;
    const double afOY    = 200.0;   // y-center of airfoil view
    const double afScale = 600.0;   // chord → pixels

    // Cp chart (bottom half)
    const double cpOX    = 80.0;
    const double cpOY    = 460.0;   // y = 0 line in Cp chart
    const double cpScaleX = 600.0;
    const double cpScaleY = 120.0;  // Cp = 1 → 120 px downward

    const double chord = airfoil_.chord() > 1e-12 ? airfoil_.chord() : 1.0;

    // ── Collect panel data ───────────────────────────────────────────────────
    const int  N   = airfoil_.numPanels();
    const auto& cp = solver_.cp();

    // Split panels into upper (n_y > 0) and lower (n_y < 0)
    std::vector<std::pair<double,double>> upper_pts, lower_pts;

    for (int i = 0; i < N; ++i)
    {
        const Vector3D ni = airfoil_.panelNormal(i);
        const double xoc  = airfoil_.panelCenter(i).x / chord;
        const double cpi  = cp[i];

        if (ni.y >= 0.0)
            upper_pts.push_back({xoc, cpi});
        else
            lower_pts.push_back({xoc, cpi});
    }

    // Sort by x/c for clean curves
    auto byX = [](const std::pair<double,double>& a, const std::pair<double,double>& b){
        return a.first < b.first;
    };
    std::sort(upper_pts.begin(), upper_pts.end(), byX);
    std::sort(lower_pts.begin(), lower_pts.end(), byX);

    // ── Open file ────────────────────────────────────────────────────────────
    std::ofstream f(filename);
    if (!f.is_open())
        throw std::runtime_error("SvgPlotter: cannot open \"" + filename + "\".");

    // Helper lambda for rounded floating-point output
    auto fmt = [](double v) -> std::string {
        std::ostringstream s;
        s << std::fixed << std::setprecision(2) << v;
        return s.str();
    };

    // ── SVG header ───────────────────────────────────────────────────────────
    f << R"(<?xml version="1.0" encoding="UTF-8"?>
<svg xmlns="http://www.w3.org/2000/svg"
     width=")" << totalW << R"(" height=")" << totalH << R"("
     viewBox="0 0 )" << totalW << " " << totalH << R"(">
<defs>
  <style>
    text { font-family: monospace; font-size: 12px; fill: #ddd; }
    .title { font-size: 16px; fill: #fff; font-weight: bold; }
    .label { font-size: 11px; fill: #aaa; }
    .axis  { stroke: #555; stroke-width: 1; fill: none; }
    .grid  { stroke: #333; stroke-width: 0.5; fill: none; stroke-dasharray: 4,4; }
  </style>
</defs>
)";

    // Background
    f << "<rect width=\"" << totalW << "\" height=\"" << totalH
      << "\" fill=\"#1a1a2e\"/>\n";

    // ── Title ────────────────────────────────────────────────────────────────
    double aoaDeg = freestream_.aoa() * 180.0 / M_PI;
    std::ostringstream titleSS;
    titleSS << std::fixed << std::setprecision(1)
            << airfoil_.name
            << "   α = " << aoaDeg << "°"
            << "   Cl = " << std::setprecision(4) << solver_.cl();

    f << "<text class=\"title\" x=\"" << (totalW / 2)
      << "\" y=\"30\" text-anchor=\"middle\">" << titleSS.str() << "</text>\n";

    // ── Airfoil panel ────────────────────────────────────────────────────────

    // Draw a light fill for the airfoil interior
    {
        f << "<polyline points=\"";
        for (int i = 0; i <= airfoil_.numNodes() - 1; ++i)
        {
            double px, py;
            double xoc = airfoil_.node(i).x / chord;
            double yoc = airfoil_.node(i).y / chord;
            airfoilToSvg(xoc, yoc, afOX, afOY, afScale, px, py);
            f << fmt(px) << "," << fmt(py) << " ";
        }
        // Close back to first node
        {
            double px, py;
            airfoilToSvg(airfoil_.node(0).x / chord,
                         airfoil_.node(0).y / chord,
                         afOX, afOY, afScale, px, py);
            f << fmt(px) << "," << fmt(py);
        }
        f << "\" fill=\"#16213e\" stroke=\"#4a90d9\" stroke-width=\"2\"/>\n";
    }

    // Draw Cp arrows on the airfoil surface
    const double arrowScale = 60.0;  // 1 Cp unit → px of arrow length
    for (int i = 0; i < N; ++i)
    {
        const double xoc = airfoil_.panelCenter(i).x / chord;
        const double yoc = airfoil_.panelCenter(i).y / chord;
        const Vector3D ni = airfoil_.panelNormal(i);
        const double cpi  = cp[i];

        double sx, sy;
        airfoilToSvg(xoc, yoc, afOX, afOY, afScale, sx, sy);

        // Arrow tip: offset along outward normal by -Cp (suction positive outward)
        // SVG y is flipped so negate y component
        double ex = sx + ni.x * (-cpi) * arrowScale;
        double ey = sy - ni.y * (-cpi) * arrowScale;

        // Colour: suction (Cp<0) = blue, pressure (Cp>0) = orange
        const char* colour = (cpi < 0.0) ? "#4a90d9" : "#e07b39";
        f << "<line x1=\"" << fmt(sx) << "\" y1=\"" << fmt(sy)
          << "\" x2=\"" << fmt(ex) << "\" y2=\"" << fmt(ey)
          << "\" stroke=\"" << colour << "\" stroke-width=\"1.5\" opacity=\"0.7\"/>\n";
    }

    // Panel label
    f << "<text class=\"label\" x=\"" << fmt(afOX)
      << "\" y=\"" << fmt(afOY + 80.0) << "\""
      << ">← chord →     (arrows: blue = suction, orange = pressure)</text>\n";

    // Chord axis line
    {
        double x0, y0, x1, y1;
        airfoilToSvg(0.0, 0.0, afOX, afOY, afScale, x0, y0);
        airfoilToSvg(1.0, 0.0, afOX, afOY, afScale, x1, y1);
        f << "<line x1=\"" << fmt(x0) << "\" y1=\"" << fmt(y0)
          << "\" x2=\"" << fmt(x1) << "\" y2=\"" << fmt(y1)
          << "\" class=\"axis\"/>\n";
    }

    // ── Cp chart ─────────────────────────────────────────────────────────────

    // Axes
    // Horizontal (x/c = 0..1)
    f << "<line x1=\"" << fmt(cpOX) << "\" y1=\"" << fmt(cpOY)
      << "\" x2=\"" << fmt(cpOX + cpScaleX) << "\" y2=\"" << fmt(cpOY)
      << "\" class=\"axis\"/>\n";
    // Vertical (Cp = -2..1)
    double cpMin = -2.0, cpMax = 1.0;
    f << "<line x1=\"" << fmt(cpOX) << "\" y1=\"" << fmt(cpOY + cpMax * cpScaleY)
      << "\" x2=\"" << fmt(cpOX) << "\" y2=\"" << fmt(cpOY + cpMin * cpScaleY)
      << "\" class=\"axis\"/>\n";

    // Grid lines at Cp = -2,-1, 0, 1
    for (double cpLine : {-2.0, -1.0, 0.0, 1.0})
    {
        double gy = cpOY + cpLine * cpScaleY;
        f << "<line x1=\"" << fmt(cpOX) << "\" y1=\"" << fmt(gy)
          << "\" x2=\"" << fmt(cpOX + cpScaleX) << "\" y2=\"" << fmt(gy)
          << "\" class=\"grid\"/>\n";
        f << "<text class=\"label\" x=\"" << fmt(cpOX - 6.0)
          << "\" y=\"" << fmt(gy + 4.0)
          << "\" text-anchor=\"end\">" << cpLine << "</text>\n";
    }
    // x/c labels
    for (double xLabel : {0.0, 0.25, 0.5, 0.75, 1.0})
    {
        double gx = cpOX + xLabel * cpScaleX;
        f << "<text class=\"label\" x=\"" << fmt(gx)
          << "\" y=\"" << fmt(cpOY + 15.0)
          << "\" text-anchor=\"middle\">" << xLabel << "</text>\n";
    }

    // Chart title
    f << "<text class=\"label\" x=\"" << fmt(cpOX + cpScaleX / 2.0)
      << "\" y=\"" << fmt(cpOY + cpMin * cpScaleY - 8.0)
      << "\" text-anchor=\"middle\">Cp distribution (blue = upper surface, red = lower)</text>\n";
    f << "<text class=\"label\" x=\"" << fmt(cpOX + cpScaleX + 5.0)
      << "\" y=\"" << fmt(cpOY + 4.0) << "\">x/c</text>\n";

    // Draw upper surface curve
    if (!upper_pts.empty())
    {
        f << "<polyline points=\"";
        for (const auto& p : upper_pts)
        {
            double px, py;
            cpToSvg(p.first, p.second, cpOX, cpOY, cpScaleX, cpScaleY, px, py);
            f << fmt(px) << "," << fmt(py) << " ";
        }
        f << "\" fill=\"none\" stroke=\"#4a90d9\" stroke-width=\"2\"/>\n";
    }

    // Draw lower surface curve
    if (!lower_pts.empty())
    {
        f << "<polyline points=\"";
        for (const auto& p : lower_pts)
        {
            double px, py;
            cpToSvg(p.first, p.second, cpOX, cpOY, cpScaleX, cpScaleY, px, py);
            f << fmt(px) << "," << fmt(py) << " ";
        }
        f << "\" fill=\"none\" stroke=\"#e05a5a\" stroke-width=\"2\"/>\n";
    }

    // Dots on curves
    auto drawDots = [&](const std::vector<std::pair<double,double>>& pts,
                        const char* colour)
    {
        for (const auto& p : pts)
        {
            double px, py;
            cpToSvg(p.first, p.second, cpOX, cpOY, cpScaleX, cpScaleY, px, py);
            f << "<circle cx=\"" << fmt(px) << "\" cy=\"" << fmt(py)
              << "\" r=\"3\" fill=\"" << colour << "\" opacity=\"0.8\"/>\n";
        }
    };
    drawDots(upper_pts, "#4a90d9");
    drawDots(lower_pts, "#e05a5a");

    // ── Footer ────────────────────────────────────────────────────────────────
    f << "<text class=\"label\" x=\"" << (totalW - 10) << "\" y=\"" << (totalH - 8)
      << "\" text-anchor=\"end\">Panelz2 — Hess &amp; Smith panel method</text>\n";

    f << "</svg>\n";

    std::cout << "SVG written to \"" << filename << "\"\n";
}
