#include "Airfoil.h"

Airfoil::Airfoil()
    :
      name("Unknown Airfoil")
{



}

Airfoil::Airfoil(std::string name, const std::vector<Point3D> &nodes)
    :
      nodes_(nodes),
      name(name)
{



}

void Airfoil::init(const std::vector<Point3D> &nodes)
{

    nodes_ = nodes;

    for(unsigned int i = 0; i < nodes_.size() - 1; ++i)
    {

        panelCenters_.push_back(0.5*(nodes_[i] + nodes_[i + 1]));

    }

}
