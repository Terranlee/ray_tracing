#ifndef __IMAGE_H__
#define __IMAGE_H__

//有关图像和纹理的类

#include <iostream>
#include <fstream>
#include <vector>
#include <cv.h>
#include <highgui.h>
#include "vec.h"
#include "color.h"

using namespace std;

const int mipmap_d = 11;
const double public_mip_bias = 0.5;

struct texture          //纹理类
{
	int tex_h;       //纹理原图的像素数量
	int tex_w;
	int counter;
	double pic_h;      //一张纹理在场景中需要覆盖多大的面积
	double pic_w;
	vec x_axis , y_axis;
	void create_mipmap();

	IplImage *tex[mipmap_d];       //储存纹理的指针
	texture() {};
	texture(const char*);
	void init_tex(vec);
	int getmipmap_index(int);
	col get_tex_plain(vec);
	col get_tex_sphere(vec , vec);

};

extern vector<texture> tex_vector;

#endif
