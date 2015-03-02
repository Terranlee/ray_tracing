#include <cstdlib>
#include "time.h"
#include "equipment.h"

using namespace std;

int main()
{
	srand((unsigned)time(NULL));
	bool type;
	int begin , end;
	tracer.traverse(0);           //注释掉的部分是我跑动画出来的时候用到的

	/*
	cin>>type;
	if(type == 0)
		tracer.traverse(0);
	else
	{
		cin>>begin>>end;
		for(int i=begin; i<end; i++)
			tracer.traverse(i);
	}
	*/
	return 0;
}
