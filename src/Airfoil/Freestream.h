#ifndef FREESTREAM_H
#define FREESTREAM_H

#include <string>
#include <map>

#include "Vector3D.h"

class Freestream
{

private:

    double rho_;
    Vector3D velocity_;

public:

    Freestream();
    Freestream(double rho, const Vector3D& velocity);
    Freestream(double rho, double magnitude, double angle);

    void changeAoA(double newAngle);
    void changeSpeed(double newSpeed);

    void acceptInput(std::map<std::string, std::string> &input);

};

#endif
