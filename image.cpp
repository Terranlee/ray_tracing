#include "image.h"

//有关图像和纹理方面的

vector<texture> tex_vector;
vec preview(0,0,0);

texture::texture(const char* file_name)
{
	for(int i=0; i<mipmap_d; i++)             //初始化纹理，尝试使用徐昆老师介绍的mipmap方式进行实现
		tex[i] = 0;
	tex[0] = cvLoadImage(file_name , 1);      //把原始的纹理初始化进来
	if(tex[0] == 0)
	{
		cout<<"load texture error"<<endl;
		exit(-1);
	}
	tex_h = tex[0]->height;
	tex_w = tex[0]->width;
	x_axis = vec(1,0,0);
	y_axis = vec(0,1,0);
}

void texture::init_tex(vec N)             //初始化纹理
{
	N.normalize();
    vec up(0 , 0 , 1);
    if(fabs(N.x) < Eps && fabs(N.y) < Eps)
        if(N.z > Eps) up = vec(0 , 1 , 0);
        else up = vec(0 , -1 , 0);
    x_axis = up.cross(N);
    y_axis = N.cross(x_axis);
    x_axis.normalize();
    y_axis.normalize();
    create_mipmap();
}

void texture::create_mipmap()          //生成mipmap的一系列小纹理
{
	counter = 1;
	CvSize new_size;
	new_size.width = tex_w;
	new_size.height = tex_h;
	int i = 1;
	while(new_size.width != 1 && new_size.height != 1)         //小纹理的尺寸每次减半，储存在一个vector中
	{
		new_size.width /= 2;
		new_size.height /= 2;
		tex[i] = cvCreateImage(new_size , tex[0]->depth , tex[0]->nChannels);
		cvResize(tex[i-1] , tex[i] , CV_INTER_LINEAR);
		counter++;
		i++;
	}
}

int texture::getmipmap_index(int dst)             //计算这个点利用mipmap中的哪一张小纹理来计算
{
	long oldS=tex_h * tex_w;
    long dstS=dst*dst;
    if (dstS>=oldS)
        return 0;
    else if (dstS<=1)
        return counter-1;
    else
        return (int)(log(oldS/dstS)*0.5+public_mip_bias);
}

col texture::get_tex_plain(vec P)             //平面纹理映射
{
	/*
	CvScalar pixel;
	double u = P*x_axis;
	double v = P*y_axis;
	double distance = (preview - P).module();    //计算上一个纹理点和这一个纹理点之间的距离，如果距离很大的话，就用小的mipmap纹理计算
	preview = P;

	int dst = int(pic_h / distance);
	int number = getmipmap_index(dst);        //计算出纹理的编号number
	int real_texh = tex_h / pow(2 , number);
	int real_texw = tex_w / pow(2 , number);
	u = u - int(u / pic_h) * pic_h;
	v = v - int(v / pic_w) * pic_w;
	if(u < 0) u += pic_h;
	if(v < 0) v += pic_w;
	int x = int(u / pic_h * real_texh);        //计算出在实际纹理图片中的坐标编号
	int y = int(v / pic_w * real_texw);
	pixel = cvGet2D(tex[number], x , y);       //mipmap实际运行的时候并没有达到想要的效果，所以还实现了普通的纹理效果计算
	return col(double(pixel.val[2])/255.0 , double(pixel.val[1])/255.0 , double(pixel.val[0])/255.0);
	*/
	
	CvScalar pixel;
	double u = P*x_axis;
	double v = P*y_axis;
	u = u - int(u / pic_h) * pic_h;
	v = v - int(v / pic_w) * pic_w;
	if(u < 0) u += pic_h;
	if(v < 0) v += pic_w;
	int x = u / pic_h * tex_h;         //计算出在实际纹理图片中的坐标编号
	int y = v / pic_w * tex_w;
	pixel = cvGet2D(tex[0], x , y);	
	return col(double(pixel.val[2])/255.0 , double(pixel.val[1])/255.0 , double(pixel.val[0])/255.0);	
}

col texture::get_tex_sphere(vec P , vec O)      //球面纹理映射
{
	CvScalar pixel;
	vec up(0,0,1);
	vec front(0,-1,0);
	vec point = P - O;            //计算出光线撞击点相对于球心的位置为point
	point.normalize();            //point标准化一下，之后在一个标准球体上进行计算

	double ceita = acos(point*up) / Pi;            //采取球坐标系进行计算，计算出ceita和phi角度
	double phi = acos(point.x / sqrt(sqr(point.x) + sqr(point.y))) / Pi;
	ceita *= pic_h , phi *= pic_w;
	ceita -= int(ceita) , phi -= int(phi);
	int x = ceita*tex_h;                            //根据ceita和phi计算出在实际纹理图片中的坐标编号
	int y = phi*tex_w;
	pixel = cvGet2D(tex[0] , x , y);
	return col(double(pixel.val[2])/255.0 , double(pixel.val[1])/255.0 , double(pixel.val[0])/255.0);
}
