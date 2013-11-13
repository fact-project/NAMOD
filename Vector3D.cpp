#include "Vector3D.h"
//======================
Vector3D::Vector3D(){
}
//======================
Vector3D::Vector3D(const double nx,const double ny,const double nz){
	x=nx; y=ny; z=nz;
}
//======================
void Vector3D::set_vec3D
(const double nx,const double ny,const double nz){
	x=nx; y=ny; z=nz;
}
//======================
double Vector3D::norm2() const{
/*!
 * Let \f$ \vec{a} \f$ be a three dimensional vector represented by
 * Vector3D. The Euclidean norm aka norm 2  aka the length of this vector
 * is given by a.norm2() which calculates:
 * \f{eqnarray*}{ \Vert \vec{a} \Vert_2 &=& \sqrt{a_x^2 + a_y^2 + a_z^2 }
 * \f}
 */
	return sqrt( pow(x,2) + pow(y,2) + pow(z,2) );
}
//======================
Vector3D Vector3D::cross_product(const Vector3D v) const{
// crossproduct
	Vector3D temp;
	temp.x = y*v.z - z*v.y;
	temp.y = z*v.x - x*v.z;
	temp.z = x*v.y - y*v.x;
	return temp;
}
//======================
double Vector3D::operator*(const Vector3D vec_two) const{
	double scalarproduct;
	scalarproduct = vec_two.x*x + vec_two.y*y + vec_two.z*z;
	return scalarproduct;
}
//======================
Vector3D Vector3D::operator*(const double scalar) const{
	Vector3D  temp;
	temp.x=x*scalar;
	temp.y=y*scalar;
	temp.z=z*scalar;
	return temp;
}
//======================
Vector3D Vector3D::operator-(const Vector3D vec_two) const{
	Vector3D temp;
	temp.x = x-vec_two.x;
	temp.y = y-vec_two.y;
	temp.z = z-vec_two.z;
	return temp;
}
//======================
Vector3D Vector3D::operator+(const Vector3D vec_two) const{
	Vector3D temp;
	temp.x = x+vec_two.x;
	temp.y = y+vec_two.y;
	temp.z = z+vec_two.z;
	return temp;
}
//======================
Vector3D Vector3D::operator/(const double scalar) const{
Vector3D temp;
	temp.x=x/scalar;
	temp.y=y/scalar;
	temp.z=z/scalar;
	return temp;
}
//======================
void Vector3D::operator=(const Vector3D eq){
	x =eq.x;
	y =eq.y;
	z =eq.z;
}
//======================
void Vector3D::disp() const{
	std::stringstream out; 
	out.str("");
	out<<"vector: ("<<x<<"|"<<y<<"|"<<z<<") [m]"<<std::endl;
	std::cout<<out.str();
}
//======================
std::string Vector3D::get_string() const{
	std::stringstream out; 
	out.str("");
	out<<"("<<x<<"|"<<y<<"|"<<z<<") [m]";
	return out.str();
}
//======================
void Vector3D::mirror(Vector3D* ray) const{
	// mirror martix
	//
	// This is taken from 
	// (OPTI 421/521 – Introductory Optomechanical Engineering)
	// J.H. Bruge
	// University of Arizona
	//
	//
	//                k1    n     k2
	//	               \    /\   /
	//                  \   |   /
	//                   \  |  /
	//                    \ | /
	// ____________________\|/______________________
	//     mirror-surface
	//
	// k1: incidate ray
	// k2: reflected ray
	// n:  surface normal
	//
	// n = [nx,ny,nz]^T
	// 
	// It can be written:
	//
	// k2 = M*k1
	//
	// M = EYE - 2*n*n^T
	//
	// using EYE =  [1 0 0]
	//				[0 1 0]
	//              [0 0 1]
	//
	// This function has to be called like:
	//
	// Vector3D vec_surface_normal;
	// Vector3D dir_of_ray_to_be_reflected;
	//
	// vec_surface_normal.mirror(dir_of_ray_to_be_reflected);
	//
	// dir_of_ray_to_be_reflected is overwritten with the reflected ray.
	//
	ray->set_vec3D(
	//x component
	(1.0-2.0*pow(x,2.0))*ray->x +
	(-2.0*x*y)*ray->y +
	(-2.0*x*z)*ray->z,
	//y component
	(-2.0*x*y)*ray->x +
	(1.0-2.0*pow(y,2.0))*ray->y +
	(-2.0*y*z)*ray->z,
	//z component
	(-2.0*x*z)*ray->x + 
	(-2.0*y*z)*ray->y + 
	(1.0-2.0*pow(z,2.0))*ray->z
	);
}
//======================
void Vector3D::set_unit_vector_x(){
	x = 1.0;
	y = 0.0;
	z = 0.0;
}
//======================
void Vector3D::set_unit_vector_y(){
	x = 0.0;
	y = 1.0;
	z = 0.0;
}
//======================
void Vector3D::set_unit_vector_z(){
	x = 0.0;
	y = 0.0;
	z = 1.0;
}
//======================
void Vector3D::set_null_vector(){
	x = 0.0;
	y = 0.0;
	z = 0.0;
}
//======================
double Vector3D::get_x() const{
	return x;
}
//======================
double Vector3D::get_y() const{
	return y;
}
//======================
double Vector3D::get_z() const{
	return z;
}
//======================
