//=================================
// include guard
#ifndef __VECTOR3D_H_INCLUDED__
#define __VECTOR3D_H_INCLUDED__

//=================================
// forward declared dependencies

//=================================
// included dependencies
#include <iostream>
#include <math.h>
#include <sstream>
//=================================
class Vector3D {
private:
double x;
double y;
double z;
public:
//======================
Vector3D();
Vector3D(const double nx,const double ny,const double nz);
//======================
void set_vec3D(const double nx,const double ny,const double nz);
//======================
double norm2() const;
//======================
Vector3D cross_product(const Vector3D v) const;
//======================
double operator*(const Vector3D vec_two) const;
//======================
Vector3D operator*(const double scalar) const;
//======================
Vector3D operator-(const Vector3D vec_two) const;
//======================
Vector3D operator+(const Vector3D vec_two) const;
//======================
Vector3D operator/(const double scalar) const;
//======================
void operator=(const Vector3D eq);
//======================
void disp() const;
//======================
std::string get_string() const;
//======================
void mirror(Vector3D* ray) const;
//======================
void set_unit_vector_x();
//======================
void set_unit_vector_y();
//======================
void set_unit_vector_z();
//======================
void set_null_vector();
//======================
double get_x() const;
//======================
double get_y() const;
//======================
double get_z() const;
//======================
friend std::ostream& operator<<(std::ostream& os, const Vector3D& vec);
};
#endif // __VECTOR3D_H_INCLUDED__ 
