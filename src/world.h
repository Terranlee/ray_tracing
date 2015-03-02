#ifndef __WORLD_H__
#define __WORLD_H__

//场景相关的类

#include <iostream>
#include <string>
#include <sstream>
#include "image.h"

using namespace std;

//物体类
struct Object{
	int type;            //不同类型的物体
	vec O , N;           //是否有球心，是否有法向量等等
	vec P[3];
	double radius , refractn;     //半径，折射率等等
	double reflect , refract , diffuse , high;        //反射光折射光等等占这个物体总发光量的多少
	bool light , texture;      //是否有纹理，是否为光源
	col color;               //是否有颜色
	int tex;                 //纹理编号是多少
	col get_tex(vec P){
		if(type == 1)
        	return tex_vector[tex].get_tex_sphere(P , O);
    	else
        	return tex_vector[tex].get_tex_plain(P);
	}
};

//相机类
struct Camera{
	vec eye;           //视点位置
	vec y_axis;        //坐标系
	vec x_axis;
	vec z_axis;
	IplImage* screen;    //屏幕

	bool if_deep;         //是否带景深效果
	double lenf;          //景深效果的焦距之多少
	bool fine_shade;      //是否带软阴影

	Camera() { eye.x=0 , eye.y=-2 , eye.z=0; 
			   y_axis.x=0 , y_axis.y=1 , y_axis.z=0;
			   if_deep = 0 , lenf = 0;
			   fine_shade = 0;
			 }
	void image_initialize();
	void image_output(int);         //输出图像
	void build_world();             //初始化场景
	void change_world(int);         //修改场景

	void set_eye(double x1, double y1, double z1)         //重新设置视点位置
		{eye.x=x1 , eye.y=y1 , eye.z=z1;}
	void set_y_axis(double x2, double y2, double z2)         //重新设置坐标系
		{y_axis.x=x2 , y_axis.y=y2 , y_axis.z=z2;}
	void set_deep(bool id , bool fs , double l)            //重新设置景深还有是否进行软阴影
		{if_deep = id , fine_shade = fs , lenf = l; }
	void cam_initialize();
};

extern vector<Object> scene;
extern Camera look;

const int sqrt_shade = 1;           //软阴影中光线簇的数量由sqrt_shade决定
const int sqrt_deep = 2;            //景深中光线簇的数量有sqrt_deep决定
const double delta = 0.4;           //偏移量的极限
const int H = 500;                  //屏幕大小
const int W = 500;

#endif
