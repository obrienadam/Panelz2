#ifndef VECTOR3D_H
#define VECTOR3D_H

#include <iostream>

class Vector3D
{

private:

public:

    Vector3D(double x = 0., double y = 0., double z = 0.);

    double x, y, z;

    double& operator()(int i);

    double mag();
    Vector3D unitVector();

    void initFromSphericalCoords(double radius, double theta, double phi);
    void initFromCylindricalCoords(double radius, double theta, double newZ);

    Vector3D& rotate(double dTheta, double dPhi);

    Vector3D& operator+=(const Vector3D& rhs);
    Vector3D& operator-=(const Vector3D& rhs);
    Vector3D& operator*=(double rhs);
    Vector3D& operator/=(double rhs);

    friend std::ostream& operator<<(std::ostream& os, const Vector3D& vector);

};

Vector3D operator+(Vector3D lhs, const Vector3D& rhs);
Vector3D operator-(Vector3D lhs, const Vector3D& rhs);
Vector3D operator*(Vector3D lhs, double rhs);
Vector3D operator*(double lhs, Vector3D rhs);
Vector3D operator/(Vector3D lhs, double rhs);

double dotProduct(const Vector3D& u, const Vector3D& v);
Vector3D crossProduct(const Vector3D& u, const Vector3D& v);
Vector3D relativeVector(const Vector3D& u, const Vector3D& v);

std::ostream& operator<<(std::ostream& os, const Vector3D& vector);

#endif
