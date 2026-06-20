#include <cmath>
#include <iostream>
#include <iomanip>

#include "Freestream.h"

Freestream::Freestream()
    :
      rho_(1.225),
      velocity_(0.0, 0.0, 0.0)
{
}

Freestream::Freestream(double rho, const Vector3D& velocity)
    :
      rho_(rho),
      velocity_(velocity)
{
}

Freestream::Freestream(double rho, double magnitude, double angleRad)
    :
      rho_(rho)
{
    velocity_.x = magnitude * std::cos(angleRad);
    velocity_.y = magnitude * std::sin(angleRad);
    velocity_.z = 0.0;
}

void Freestream::changeAoA(double newAngleRad)
{
    velocity_.initFromCylindricalCoords(velocity_.mag(), newAngleRad, 0.0);
}

void Freestream::changeSpeed(double newSpeed)
{
    velocity_ *= newSpeed / velocity_.mag();
}

void Freestream::acceptInput(std::map<std::string, std::string>& input)
{
    using namespace std;

    double metricConversion, radianConversion;

    if (input["FreestreamVelocityUnits"] == "m/s")
    {
        metricConversion = 1.0;
    }
    else
    {
        throw "Invalid velocity units in Freestream::acceptInput (only 'm/s' is supported).";
    }

    if (input["AngleOfAttackUnits"] == "degrees")
    {
        radianConversion = M_PI / 180.0;
    }
    else if (input["AngleOfAttackUnits"] == "radians")
    {
        radianConversion = 1.0;
    }
    else
    {
        throw "Invalid angle units in Freestream::acceptInput (use 'degrees' or 'radians').";
    }

    rho_ = stod(input["FreestreamDensity"]);
    velocity_.initFromCylindricalCoords(
        metricConversion * stod(input["FreestreamVelocity"]),
        radianConversion * stod(input["AngleOfAttack"]),
        0.0);

    cout << setfill('-') << setw(64) << "" << "\n"
         << "Freestream density:  " << rho_     << " kg/m³\n"
         << "Freestream velocity: " << velocity_ << " m/s\n"
         << "Angle of attack:     " << (aoa() * 180.0 / M_PI) << "°\n"
         << setfill('-') << setw(64) << "" << "\n";
}
