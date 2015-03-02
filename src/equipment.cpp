#include "equipment.h"
//仪器相关的文件
//Raytrace是光线追踪部分的类
//Rend是消除锯齿方面的类

Rend render;
Ray_trace tracer;

Rend::Rend()
{
    memset(crash_num , 0 , sizeof(int)*H*W);
    num = 0;
}

bool Rend::check(int row , int clm)          //检查某一个点是否需要进行抗锯齿处理
{
	CvScalar comp , stand;
	stand = cvGet2D(look.screen , row , clm);
	int delta;
    for(int i=row-1; i<=row+1; i++)
        for(int j=clm-1; j<clm+1; j++)         //遍历某一条光线周围的3*3根光线进行检查
        {
        	delta = 0;
            if(crash_num[i][j] != crash_num[row][clm])       //如果和周围光线是相交在不同的物体上了，那么就是在边缘，肯定要进行抗锯齿处理
                return 1;
            comp = cvGet2D(look.screen , i , j);
            for(int i=0; i<3; i++)
            	delta += (comp.val[i] - stand.val[i]) * (comp.val[i] - stand.val[i]);      //或者和周围光线颜色差别太大，也要进行抗锯齿
            if(delta > 1000)
            	return 1;
        }
    return 0;
}

void Rend::refine()
{
	int ccounter = 0;
	IplImage* ima = look.screen;
    int counter = (sqrt_rend*2+1) * (sqrt_rend*2+1);
    for(int row=1; row<H-1; row++)
        for(int clm=1; clm<W-1; clm++)
            if(check(row , clm))          //如果需要进行抗锯齿处理的话
            {       
	            ccounter++;         
                CvScalar color , temp;
                for(int i=0; i<4; i++) color.val[i] = 0;
                for(int i=-sqrt_rend; i<=sqrt_rend; i++)
                    for(int j=-sqrt_rend; j<=sqrt_rend; j++)
                    {
                        temp = cvGet2D(ima , row + i , clm + j);
                        for(int i=0; i<4; i++) color.val[i] += temp.val[i];      //就把它周围若干个点的颜色取平均值作为这一点的颜色
                    }
                for(int i=0; i<4; i++)
                    color.val[i] /= counter;
                cvSet2D(ima , row , clm , color);
            }
            cout<<ccounter<<endl;
}

void Ray_trace::image_release()          //释放掉image
{
	for(int i=0; i<tex_vector.size(); i++)
		for(int j=0; j<mipmap_d; j++)
			cvReleaseImage(&(tex_vector[i].tex[j]));
}

int Ray_trace::intersect(vec eye , vec dir , int i, vec & nP , vec & nN)        //求交过程，详见vec.cpp
{
	int check = false;
	if(scene[i].type == 1)
		check = eye.inter_sphere(dir , scene[i].O , scene[i].radius , nP , nN);
	else if(scene[i].type == 2)
		check = eye.inter_triangle(dir , scene[i].N , scene[i].radius , scene[i].P , nP , nN);
	else if(scene[i].type == 3)
		check = eye.inter_plain(dir , scene[i].N , scene[i].radius , nP , nN);
	return check;
}

col Ray_trace::ray_tracing(vec eye , vec dir , int depth , double matter)          //光线追踪过程
{
	col ans(0,0,0);
	if(depth > max_dep)              //如果深度大于预设的深度，就返回黑色
		return ans;
	
	int crash = 0 , id = -1; 
	vec P , N , tmpP , tmpN; 
	dir.normalize();
	for(int i=0; i<scene.size() ; i++)          //对场景中的每一个物体进行求交运算，而且还要找出距离最近的那一个物体是谁
	{
		int check = 0;
		check = intersect(eye , dir , i , tmpP , tmpN);
		if(check != 0 && (id==-1 || (tmpP-eye).module() < (P-eye).module() )){           //判断出最近的物体是哪一个
			crash = check , id = i , P = tmpP , N = tmpN;
		}
	}
	if(crash == 0 )                   //如果没有求交到光线，那么就直接返回一个黑色
		return ans;	
	if(depth == 1)                    //如果深度为1的话，就要set求交到哪个物体上了，为之后的抗锯齿做准备
		render.setnum(id);
	if(scene[id].light == true)       //如果求交到光源上了，那么就返回光源的颜色
	{
		ans = scene[id].color;
		return ans;
	}

	col color;                       //之后就是求交到物体上了，先判断一下这个物体到底是颜色的还是纹理的
	if(scene[id].texture)
		color = scene[id].get_tex(P);
	else
		color = scene[id].color;

	for(int li=0; li<scene.size(); li++)    //判断遮挡关系，是否有阴影
	{
		if(scene[li].light == true)
		{
			vec Lvec = scene[li].O - P;
			double shade;
			if(depth == 1 && look.fine_shade)             //fine_shade，生成软阴影
			{
				int counter = (sqrt_shade*2 + 1) * (sqrt_shade*2 + 1);  //软阴影，碰撞点和光源连线，产生counter跟光线，向多个方向产生一个偏移量
																		//然后和其他物体求交，判断遮挡关系
				shade = double(1) / double(counter);
				vec deltax , deltay;                     //产生偏移
				if(scene[li].type == 1){
					deltax = vec(1,0,0) * scene[id].radius * 2;
					deltay = vec(0,1,0) * scene[id].radius * 2;
				}
				else if(scene[li].type == 2 || scene[id].type == 4){
					deltax = scene[li].O - scene[li].P[0];
					deltay = scene[li].O - scene[li].P[1];
				}
				deltax = deltax / sqrt_shade;
				deltay = deltay / sqrt_shade;

				for(int i=-sqrt_shade; i<=sqrt_shade; i++)        //对所有产生偏移的光线和场景中其他物体进行求交，判断遮挡
					for(int j=-sqrt_shade; j<=sqrt_shade; j++)
					{
						vec sh_dir = scene[li].O + deltax*i*(double(rand()) / RAND_MAX);      //偏移量在给定范围deltax/deltay的基础上随机产生
						sh_dir += deltay*j*(double(rand()) / RAND_MAX);
						sh_dir = sh_dir - P;
						for(int j=0; j<scene.size(); j++)
							if(!scene[j].light && j != id && intersect(P , sh_dir, j , tmpP , tmpN) != 0)
								if(Lvec.module() > (tmpP-P).module())           //和其他物体求交，看看有没有被物体挡住
									counter--;
					}
				shade *= counter;
			}
			else            //没有软阴影效果的普通阴影，只用一条光线和其他物体求交，判断遮挡
			{				
				shade = 1;
				for(int j=0; j<scene.size(); j++)
					if(!scene[j].light && id != j && intersect(P , Lvec , j , tmpP , tmpN) != 0)
						if(Lvec.module() > (tmpP-P).module())           
						{
							shade = 0;
							break;
						}
			}
			Lvec.normalize();                  
			if(shade != 0)                     //普通漫反射，只由某一点的法向量和到光源的连线决定
			{
				if(scene[id].diffuse > Eps)	{
					double coscita = Lvec*N;
					if(coscita > Eps)
						ans += color*scene[li].color*scene[id].diffuse*coscita*shade;
					if(scene[id].high > Eps)         //phong模型的高光效果，由视线/法向量/到光源连线共同决定
					{
						vec reflect = Lvec-N*2*coscita;
						double highlight = reflect*dir;
						if(highlight > Eps)
							ans += color*scene[li].color*scene[id].high*pow(highlight , pow_high)*shade;
					}
				}
			}
		}
	}

	if(scene[id].reflect > Eps)             //计算反射光，进行递归
	{
		vec dir_ref = dir - (N*(2*(dir*N)));
		col ref_col = ray_tracing(P , dir_ref , depth+1 , matter);
		ans += ref_col*scene[id].reflect*color;
	}

	double refr = scene[id].refract;           //计算折射光
	if(scene[id].refract > Eps)
	{
		double in_refractn = scene[id].refractn;
		if(crash == -1) in_refractn = 1;
		double n = matter / in_refractn;
		double cosin = -(dir*N);
		double sinout2 = 1 - sqr(n) * (1 - sqr(cosin));         //计算折射的角度，进行递归
		if(sinout2 > Eps)
		{
			double sinout = sqrt(sinout2);
        	vec out = dir * n + N * ( n * cosin - sinout );
	        	col rcol = ray_tracing( P , out , depth + 1 , in_refractn);
        	ans += rcol * refr * color;
		}
	}
	ans.limitation();
	return ans;
}

void Ray_trace::traverse(int num)       //遍历屏幕上的每一个像素点，进行光线追踪
{
	look.build_world();                  //初始化场景
	look.cam_initialize();               //初始化相机
	look.change_world(num);              //改变场景(生成动画的时候用到的)
	look.image_initialize();             //初始化屏幕
	double counter = (sqrt_deep*2+1) * (sqrt_deep*2+1);
	IplImage* ima = look.screen;
	for ( int row = 0 ; row < H ; row++ ) //H是所渲染图片的高上的像素数
	{
		cout<<row<<endl;
		uchar* ptr = (uchar*) ima->imageData + row*(ima->widthStep);
    	for ( int clm = 0 ; clm < W ; clm++ )
    	{
    		col point;
    		vec dir(look.y_axis.x , look.y_axis.y , look.y_axis.z);
    		render.setzero();
    		if(look.if_deep)            //生成景深效果
    		{
    			double x = 1-double(clm)*2/double(W) , z = 1-double(row)*2/double(H);
				dir += look.x_axis*x;
				dir += look.z_axis*z;             //先根据在屏幕上的位置计算出光线的方向

    			double times = look.lenf / dir.y;
    			dir = dir * times;
    			vec hit_point = look.eye + dir;      //根据焦距的大小，计算出在焦平面上的碰撞点是哪个

    			for(int i=-sqrt_deep; i<=sqrt_deep; i++ )
    				for(int j=-sqrt_deep; j<=sqrt_deep; j++)
    				{                     //产生一簇光线，每一个都有一个偏移量，原理和软阴影的类似
    					vec begin = look.eye + look.x_axis * i * delta * double(rand()) / RAND_MAX;   
    					begin += look.z_axis * j * delta * double(rand()) / RAND_MAX;
    					dir = hit_point - begin;           //这些光线最终都会在焦平面上汇聚到之前的hit_point那一点上，可以计算出光线的方向
    					point += ray_tracing(begin , dir , 1 , 1);
    				}
    			point = point / counter;
    		}
    		else{
    			double x = 1-double(clm)*2/double(W) , z = 1-double(row)*2/double(H);       //没有景深效果，就是一根光线求交
				dir += look.x_axis*x;
				dir += look.z_axis*z;
    			point = ray_tracing(look.eye , dir , 1,1);
			}
			render.setcrash(row , clm);
			point.limitation();
			ptr[3*clm+0] = (uchar)(point.b*255);
			ptr[3*clm+1] = (uchar)(point.g*255);
			ptr[3*clm+2] = 	(uchar)(point.r*255);		
    	}
    }
    cout<<"end of traverse"<<endl;
    render.refine();                    //进行抗锯齿处理
    look.image_output(num);             //输出照片
    image_release();
}
