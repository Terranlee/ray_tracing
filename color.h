#ifndef __COLOR_H__
#define __COLOR_H__

//有关颜色的类
//包括了rgb三个颜色值以及各种可能用到的操作符，仿照Vec3f中的方式写的
#include <iostream>
#include <cstdio>

using namespace std;

class col{
public:
	double r , g , b;
	col() {}
	col(double R , double G , double B) {r = R, g = G , b = B;}
	col operator* (const double k){
		return col(r*k , g*k , b*k);
	}
	col operator* (const col op){
		return col(r*op.r , g*op.g , b*op.b);
	}
	col operator+ (const col op){
		return col(r+op.r , g+op.g , b+op.b);
	}
	col operator/ (const double k){
		return col(r/k , g/k , b/k);
	}
	col operator= (const col op){
		r = op.r , g = op.g , b = op.b;
		return *this;
	}
	void operator+= (const col op){
		r += op.r , g += op.g , b += op.b;
	}
	void limitation()
	{
		if(r > 1) r=1;
		if(g > 1) g=1;
		if(b > 1) b=1;
	}
};

#endif
