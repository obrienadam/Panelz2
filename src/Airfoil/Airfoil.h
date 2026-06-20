#ifndef AIRFOIL_H
#define AIRFOIL_H

#include <string>
#include <vector>
#include <cmath>

#include "Point3D.h"

class Airfoil
{
public:
    Airfoil();
    Airfoil(std::string name, const std::vector<Point3D>& nodes);

    void init(const std::vector<Point3D>& nodes);

    // ── Basic accessors ──────────────────────────────────────────────────────

    std::string name;

    int  numNodes()  const { return static_cast<int>(nodes_.size()); }
    int  numPanels() const { return static_cast<int>(nodes_.size()) - 1; }

    const Point3D& node(int i) const { return nodes_[i]; }

    // ── Panel geometry ───────────────────────────────────────────────────────

    /// Midpoint of panel i
    const Point3D& panelCenter(int i) const { return panelCenters_[i]; }

    /// Euclidean length of panel i
    double panelLength(int i) const;

    /// Angle of panel i tangent w.r.t. the positive x-axis [radians]
    double panelAngle(int i) const;

    /// Unit tangent vector of panel i (pointing from node i to node i+1)
    Vector3D panelTangent(int i) const;

    /// Outward unit normal of panel i.
    /// For a CCW-oriented airfoil (UIUC format), this is the tangent rotated
    /// -90° so that it points away from the airfoil interior.
    Vector3D panelNormal(int i) const;

    /// Chord length (max x − min x over all nodes)
    double chord() const;

private:
    std::vector<Point3D> nodes_;
    std::vector<Point3D> panelCenters_;
};

#endif // AIRFOIL_H
