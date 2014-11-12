#ifndef AIRFOIL_H
#define AIRFOIL_H

#include <string>
#include <vector>

#include "Point3D.h"

class Airfoil
{

private:

    std::vector<Point3D> nodes_;
    std::vector<Point3D> panelCenters_;

public:

    Airfoil();
    Airfoil(std::string name, const std::vector<Point3D>& nodes);

    void init(const std::vector<Point3D>& nodes);

    std::string name;

};

#endif
