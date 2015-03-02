#include <iostream>
#include "highgui.h"
#include "cv.h"
#include <string>
#include <vector>
#include <sstream>
#include <stdio.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>

using namespace std;

stringstream convert;
vector<string> filevec;
vector<int> intvec;

void get_pics(string filepath)         //获得文件夹下的所有文件名
{
	string name;
	DIR *dp;
	struct dirent *dirp;
	if((dp=opendir(filepath.data()))==NULL)
		cout<<"can't open filepath"<<endl;
	while ((dirp=readdir(dp))!=NULL)
	{
		name = string(dirp->d_name);
		if(name != "." && name != ".." && name != "main" && name != "movie_maker.cpp" && name != "makefile")
			filevec.push_back(name);
	}
	closedir(dp);
}

void sort_name()
{
	for(int i=0; i<filevec.size(); i++)
	{
		int position = filevec[i].find_first_of(".");
		string substring = filevec[i].substr(0 , position);
		convert.clear();
		convert<<substring;
		convert>>position;
		intvec.push_back(position);
	}
	for(int i=0; i<intvec.size(); i++)
		for(int j=i; j<intvec.size(); j++)
			if(intvec[i] > intvec[j])
			{
				swap(intvec[i] , intvec[j]);
				swap(filevec[i] , filevec[j]);
			}
}

void preprocessing()
{
	char *buffer;  
    if((buffer = getcwd(NULL, 0)) == NULL)  
        perror("getcwd error");  
    else  
        cout<<buffer<<endl;
    string cwdpath(buffer);
    get_pics(cwdpath);
    sort_name();
}

int main()
{
	preprocessing();

	int iscolor = 1;
	double fps = 30;
	int imgW = 500;
	int imgH = 500;
	char* video_name = "ray_tracing.avi";
	int startframe = 0;
	int endframe = 286;


	CvSize size = cvSize(imgW , imgH);
	CvVideoWriter* pWriter = cvCreateVideoWriter(video_name , CV_FOURCC('X','V','I','D'), fps , size);
	IplImage* pImg = NULL;
	while(startframe <= endframe)
	{
		pImg = cvLoadImage(filevec[startframe].data() , iscolor);
		if(!pImg)
		{
			cout<<"can't open file"<<endl;
			exit(-1);
		}
		cvWriteFrame(pWriter , pImg);
		cvWaitKey(1);
		cout<<"write frame"<<startframe<<" to the video"<<endl;
		startframe++;
		cvReleaseImage(&pImg);
	}
	cvReleaseVideoWriter(&pWriter);
 	return 0;
}