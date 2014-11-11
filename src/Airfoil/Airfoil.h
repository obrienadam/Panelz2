#ifndef AIRFOIL_H
#define AIRFOIL_H

#include <string>
#include <vector>

#include "Point3D.h"
#include "Input.h"

class Airfoil
{

private:

    std::vector<Point3D> nodes_;
    std::vector<Point3D> panelCenters_;

public:

    Airfoil();
    Airfoil(int argc, const char* argv[]);

    std::string name;

    Input input;

};

#endif
