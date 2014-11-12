#include <math.h>
#include <iostream>
#include <iomanip>

#include "Freestream.h"

Freestream::Freestream()
{

}

Freestream::Freestream(double rho, const Vector3D& velocity)
    :
      rho_(rho),
      velocity_(velocity)
{

}

Freestream::Freestream(double rho, double magnitude, double angle)
    :
      rho_(rho)
{

    velocity_.x = magnitude*cos(angle);
    velocity_.y = magnitude*sin(angle);

}

void Freestream::changeAoA(double newAngle)
{

    velocity_.initFromCylindricalCoords(velocity_.mag(), newAngle, 0.);

}

void Freestream::changeSpeed(double newSpeed)
{

    velocity_ *= newSpeed/velocity_.mag();

}

void Freestream::acceptInput(std::map<std::string, std::string> &input)
{
    using namespace std;

    double metricConversion, radianConversion;

    if(input["FreestreamVelocityUnits"] == "m/s")
    {

        metricConversion = 1.;

    }
    else
    {

        throw "Invalid metric conversion unit in Freestream::acceptInput.";

    }

    if(input["AngleOfAttackUnits"] == "degrees")
    {

        radianConversion = M_PI/180.;

    }
    else if(input["AngleOfAttackUnits"] == "radians")
    {

        radianConversion = 1.;

    }
    else
    {

        throw "Invalid radian conversion unit in Freestream::acceptInput.";

    }

    rho_ = stod(input["FreestreamDensity"]);
    velocity_.initFromCylindricalCoords(metricConversion*stod(input["FreestreamVelocity"]),
            radianConversion*stod(input["AngleOfAttack"]), 0.);

    cout << setfill('-') << setw(64) << "" << endl
         << "Freestream density: " << rho_ << " kg/m^3\n"
         << "Freestream velocity: " << velocity_ << " m/s\n"
         << setfill('-') << setw(64) << "" << endl;

}
