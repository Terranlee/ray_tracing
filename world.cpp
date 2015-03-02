#include "world.h"

//场景相关的代码

Camera look;
vector<Object> scene;
double ceita = 0;

//初始化相机
void Camera::cam_initialize()
{
	y_axis.normalize();
	vec up(0,0,1);
	x_axis = up.cross(y_axis);
	z_axis = y_axis.cross(x_axis);
	x_axis.normalize();
	z_axis.normalize();
}

void Camera::image_initialize()
{
	screen = cvCreateImage(cvSize(H , W) , IPL_DEPTH_8U , 3);  
	if(screen == 0)
	{
		cout<<"create image error"<<endl;
		exit(-1);
	}
	cvZero(screen);
}

void Camera::image_output(int num) 
{
	stringstream str;
	string file_name;
	str<<num;
	str>>file_name;
	file_name += ".bmp";
	cout<<"begin_save"<<endl;
	cvSaveImage(file_name.data() , screen);
	cout<<"end_save"<<endl;
	cvReleaseImage(&screen);
	cout<<"end_release"<<endl;
}

//初始化场景
void Camera::build_world()
{
	ifstream fin("content.txt");       //从content.txt中读入场景信息
	string comment;
	while(getline(fin , comment))
		if(comment == "BEGIN:")
			break;
	double x , y , z;
	fin>>x>>y>>z;                       //场景信息最开始是有关相机的一些参数：视点，坐标系，是否有景深，软阴影等等
	look.set_eye(x , y , z);
	fin>>x>>y>>z;
	look.set_y_axis(x , y , z);
	bool id , fs;
	fin>>id>>fs;
	if(id){
		fin>>x;
		look.set_deep(id , fs , x);
	}
	else
		look.set_deep(id , fs , 0);

	string name;
	bool islight , istexture , iscolor , ismaterial;
	while(fin>>name)                                    //之后读入每一个物体，首先是类型
	{
		Object tmp;
		fin>>islight>>istexture>>iscolor>>ismaterial;      //然后是四个bool参数：是否为光源，是否有纹理，是否有颜色，是否有材料属性
		tmp.light = islight;
		tmp.texture = istexture;
		if(name == "sphere")                               //再根据不同的物体类型，读入位置信息，比如球心，法向量等等
		{
			tmp.type = 1;
			fin>>tmp.O;
			fin>>tmp.radius;
		}
		else if(name == "triangle")
		{
			tmp.type = 2;
			for(int i=0; i<3; i++)
				fin>>tmp.P[i];
			vec e0 = tmp.P[0] - tmp.P[1];
			vec e1 = tmp.P[1] - tmp.P[2];
			tmp.N = e0.cross(e1);
		}
		else if(name == "plain")
		{
			tmp.type = 3;
			fin>>tmp.N;
			fin>>tmp.radius;
		}
		if(istexture)                               //如果有纹理的话，就初始化一个纹理
		{
			string file_name;
			fin>>file_name;
			texture tex(file_name.data());
			fin>>tex.pic_h>>tex.pic_w;
			tex.init_tex(tmp.N);
			tex_vector.push_back(tex);
			tmp.tex = tex_vector.size()-1;
		}
		if(iscolor)                                //如果有颜色的话，就输入颜色
		{
			fin>>tmp.color.r>>tmp.color.g>>tmp.color.b;
		}
		if(ismaterial)                              //如果有材质属性就输入不同的材质属性
		{
			fin>>tmp.reflect>>tmp.refract>>tmp.diffuse>>tmp.high;
			if(tmp.refract)
				fin>>tmp.refractn;
		}
		scene.push_back(tmp);
	}
	fin.close();
}

void Camera::change_world(int num)                  //改变场景，为渲染动画做准备
{
	cout<<num<<endl;
	if(-51 < num < 0)
	{
		int c = -num;
		set_eye(0 , -15 , 2 + c*0.02*2);
		set_y_axis(0 , 1 , -0.3*c*0.02);
		cam_initialize();
		scene[9].O.z -= (c*0.02*3);
		scene[10].O.z -= (c*0.02*3);
		scene[7].O.y -= (c*0.02*2);
		scene[8].O.x += (c*0.02*1.5);
	}
	if(num == 0)
		return;
	if(num > 0 && num < 47)
	{
		ceita = num*1.5 / 90.0 *0.5*Pi;
		set_eye(-15*sin(ceita) , -15*cos(ceita) , 2);
		set_y_axis(sin(ceita) , cos(ceita) , 0);
		cam_initialize();
		scene[7].O.y -= float(num*2.0 / 60.0);
		scene[7].O.z += float(num*2.0 / 60.0);
		scene[9].radius += float(num*1.0 / 60.0);
		scene[9].O.y -= float(num*4.0 / 60.0);
		scene[9].O.z -= float((num-47)*5.0 / 60.0);
		scene[10].O.z -= float(num*6.0 / 60.0);
		scene[10].O.y -= float((num-11)*3.0 / 60.0);
	}
	if(num >= 47 && num < 67)
		scene[0].O.x += (num - 47) * 0.7;
	if(num >= 67 && num < 87)
	{
		scene[0].O.x += 12.6;
		scene[0].O.x -= (num - 67) * 0.7;
		scene[0].O.y += (num - 67) * 0.4;
	}
	if(num >=87 && num <= 115)
	{
		scene[0].O.x += 12.6;
		scene[0].O.x -= (86 - 67) * 0.7;
		scene[0].O.y += (86 - 67) * 0.4;	
		Object temp;
		temp.type = 1;
		temp.O.x = 0.0 , temp.O.y = -6.0 , temp.O.z = 15.0;
		temp.radius = 0.1;
		temp.light = true;
		temp.color.r = 1.0 , temp.color.g = 1.0 , temp.color.b = 1.0;
		scene.push_back(temp);
		scene[0].O.y -= (num - 90) * 0.4;
		scene[scene.size()-1].O.y += (num - 90) * 0.4;
	}
	if(num > 115 && num <= 140)
	{
		scene[0].O.x += 12.6;
		scene[0].O.x -= (86 - 67) * 0.7;
		scene[0].O.y += (86 - 67) * 0.4;	
		Object temp;
		temp.type = 1;
		temp.O.x = 0.0 , temp.O.y = -6.0 , temp.O.z = 15.0;
		temp.radius = 0.1;
		temp.light = true;
		temp.color.r = 1.0 , temp.color.g = 1.0 , temp.color.b = 1.0;
		scene.push_back(temp);
		scene[0].O.y -= (115 - 90) * 0.4;
		scene[scene.size()-1].O.y += (115 - 90) * 0.4;
		scene[0].O.x += (num - 115) * 0.3;
		scene[scene.size()-1].O.x -=(num - 115) * 0.3;
	}
	if(num > 140 && num <= 180)
	{
		scene[3].O.y -= (num - 140) * 0.075;
		scene[4].O.y -= (num - 140) * 0.025;
		scene[5].O.y += (num - 140) * 0.025;
		scene[6].O.y += (num - 140) * 0.075;
	}
	if(num > 180 && num <= 240)
	{
		scene[3].O.y -= (180 - 140) * 0.075;
		scene[4].O.y -= (180 - 140) * 0.025;
		scene[5].O.y += (180 - 140) * 0.025;
		scene[6].O.y += (180 - 140) * 0.075;
		look.set_deep(1 , 0 , 10 - double(num-180) / double(10) * 0.666667);
	}
}
