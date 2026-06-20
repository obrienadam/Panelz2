#include <algorithm>
#include <cmath>
#include <limits>

#include "Airfoil.h"

Airfoil::Airfoil()
    :
      name("Unknown Airfoil")
{
}

Airfoil::Airfoil(std::string name, const std::vector<Point3D>& nodes)
    :
      name(name)
{
    init(nodes);
}

void Airfoil::init(const std::vector<Point3D>& nodes)
{
    nodes_ = nodes;
    panelCenters_.clear();
    panelCenters_.reserve(nodes_.size() - 1);

    for (int i = 0, n = static_cast<int>(nodes_.size()) - 1; i < n; ++i)
        panelCenters_.push_back(0.5 * (nodes_[i] + nodes_[i + 1]));
}

double Airfoil::panelLength(int i) const
{
    const Point3D& a = nodes_[i];
    const Point3D& b = nodes_[i + 1];
    double dx = b.x - a.x;
    double dy = b.y - a.y;
    return std::sqrt(dx * dx + dy * dy);
}

double Airfoil::panelAngle(int i) const
{
    const Point3D& a = nodes_[i];
    const Point3D& b = nodes_[i + 1];
    return std::atan2(b.y - a.y, b.x - a.x);
}

Vector3D Airfoil::panelTangent(int i) const
{
    double theta = panelAngle(i);
    return Vector3D(std::cos(theta), std::sin(theta), 0.0);
}

Vector3D Airfoil::panelNormal(int i) const
{
    // Outward normal for CCW-oriented airfoil:
    // rotate tangent by -90°  →  (sin θ, -cos θ)
    double theta = panelAngle(i);
    return Vector3D(std::sin(theta), -std::cos(theta), 0.0);
}

double Airfoil::chord() const
{
    if (nodes_.empty())
        return 0.0;

    double xMin = nodes_[0].x, xMax = nodes_[0].x;
    for (const auto& p : nodes_)
    {
        xMin = std::min(xMin, p.x);
        xMax = std::max(xMax, p.x);
    }
    return xMax - xMin;
}
