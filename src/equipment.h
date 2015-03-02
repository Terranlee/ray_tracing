#ifndef __EQUIPMENT_H__
#define __EQUIPMENT_H__

//仪器类，包括光线追踪的raytrace和抗锯齿的rend
#include <fstream>
#include "world.h"
#include "memory.h"

using namespace std;

class Rend
{
private:
	int crash_num[H][W];       //碰撞到那个物体上了，用一个矩阵进行记录
	int num;
	bool check(int x , int y);       //检测是否需要抗锯齿处理

public:
	Rend();
	void setzero() { num = 0; }
	void setnum(int n) { num = n; }
	void setcrash(int h , int w) { crash_num[h][w] = num; }
	void refine();         //抗锯齿处理
};

class Ray_trace
{
private:
	col ray_tracing(vec , vec , int , double);
	int intersect(vec , vec , int , vec& , vec&);
	void image_release();

public:
	Ray_trace() {}
	void traverse(int);
};

extern Rend render;
extern Ray_trace tracer;

const double pow_high = 50;          //phong模型的高光，是50次方的
const int max_dep = 4;               //光线追踪的最大深度
const int sqrt_rend = 1;             //软阴影和景深，光线簇的数量通过sqrt_rend控制

#endif
