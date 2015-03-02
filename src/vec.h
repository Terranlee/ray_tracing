#ifndef __VEC_H__
#define __VEC_H__

//向量类，既可以表示空间中的一个点，也可以表示空间中的一个光线

#include <iostream>
#include <string>
#include <cmath>
#include "color.h"

//仿照Vec3f.cpp中进行实现

using namespace std;

const double Eps = 1e-6;
const double Pi = 3.1415927;

inline double sqr(double x){
	return x*x;
}

struct vec{
	double x , y , z;
	vec() {}
	vec(double X , double Y , double Z) {x = X , y = Y , z = Z;}
	vec operator+ (const vec op){
		return vec(x+op.x , y+op.y , z+op.z);
	}
	vec operator- (const vec op){
		return vec(x-op.x , y-op.y , z-op.z);
	}
	vec operator* (const double k){
		return vec(x*k , y*k , z*k);
	}
	vec operator/ (const double k){
		return vec(x/k , y/k , z/k);
	}
	double operator* (const vec op){
		return x*op.x + y*op.y + z*op.z;
	}
	void operator+= (const vec op){
		x += op.x , y += op.y , z += op.z;
	}
	friend istream& operator>>(istream& , vec&);
	vec cross(const vec op){
		return vec(op.y*z - op.z*y , op.z*x - op.x*z , op.x*y - op.y*x);
	}
	vec inversion(){
		return vec(-x , -y , -z);
	}
	double module(){
		return sqrt( sqr(x) + sqr(y) + sqr(z));
	}
	void normalize(){
		double length = this->module();
		x /= length , y /= length , z /= length;
	}
	int inter_sphere(vec , vec , double , vec& , vec&);
	int inter_plain(vec , vec , double , vec& , vec&);
	int inter_triangle(vec , vec , double , vec[] , vec& , vec&);
};

#endif
