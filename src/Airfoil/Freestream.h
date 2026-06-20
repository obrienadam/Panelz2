#ifndef FREESTREAM_H
#define FREESTREAM_H

#include <cmath>
#include <string>
#include <map>

#include "Vector3D.h"

class Freestream
{
public:
    Freestream();
    Freestream(double rho, const Vector3D& velocity);
    Freestream(double rho, double magnitude, double angleRad);

    // ── Mutators ─────────────────────────────────────────────────────────────

    void changeAoA(double newAngleRad);
    void changeSpeed(double newSpeed);
    void acceptInput(std::map<std::string, std::string>& input);

    // ── Accessors ─────────────────────────────────────────────────────────────

    double rho()   const { return rho_; }
    double speed() const { return const_cast<Vector3D&>(velocity_).mag(); }

    /// Angle of attack in radians (measured from +x axis)
    double aoa()   const { return std::atan2(velocity_.y, velocity_.x); }

    const Vector3D& velocity() const { return velocity_; }

private:
    double   rho_;
    Vector3D velocity_;
};

#endif // FREESTREAM_H
