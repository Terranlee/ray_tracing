#include "vec.h"

//重载流运算符，方便输入
istream& operator>> (istream& in , vec& v)
{
	in>>v.x>>v.y>>v.z;
	return in;
}

int vec::inter_sphere(vec Rl , vec Sc , double r , vec &nP , vec& nN)      //和球体求交，根据书上那种不需要解方程的方法实现的
{
	Rl.normalize();
	vec Loc = Sc - *this;
	double Tca = Loc * Rl;
	double Thc = sqr(r) - (Loc * Loc) + Tca*Tca , l = 0;    
	int ret = 0;
	if (Thc > Eps) {
		Thc = sqrt(Thc);
		double i1 = Tca - Thc , i2 = Tca + Thc;
		if (i2 < Eps) return 0; 
		if (i1 > Eps) l = i1 , ret = 1;
			else l = i2 , ret = -1;       //相交方式为-1时碰撞方式为反着的
						 //可能出现在光源在球体内部的情况
	}
	if (l < Eps) return 0;
	nP = *this + Rl * l;         //nP就是最终相交光线的长度和方向
	nN = nP - Sc;                  //nN就是相交那一点的法向量
	nN.normalize();
	if (ret == -1) nN = nN.inversion();
	return ret;
}

int vec::inter_plain(vec Rl , vec N , double dist , vec& nP , vec& nN)     //和平面求交
{
	N.normalize();
	Rl.normalize();
	double d = N * Rl;                    //两个归一化向量，d就是cos(x)
	if (fabs(d) < Eps) return 0;          //平行的情况就不相交了
	double l = (N * dist - *this) * N / d;
	if (l < Eps) return 0;
	nP = *this + Rl * l;
	if (d < 0) nN = N; else nN = N.inversion();
	if (d < 0) return 1;
	return -1;
}

int vec::inter_triangle(vec Rl , vec N , double r , vec P[] , vec& nP , vec& nN)      //和三角形求交
{
	Rl.normalize();
	N.normalize();
	double d = N * Rl;
	if( fabs(d) > Eps) return 0;
	double l = (N * r - *this) * N / d;
	if( l < 0) return 0;
	nP = *this + Rl * l;
	vec sumv = (P[1] - P[0]).cross(P[2] - P[0]);
	vec stdv = (nP - P[0]).cross(P[2] - P[0]);
	sumv.normalize() , stdv.normalize();
	double sum = sumv.module() , std = stdv.module();
	if(sum > std) return 0;
	if(d < 0) nN = N; else nN = N.inversion();
	if(d < 0) return 1;
	return -1;
}
