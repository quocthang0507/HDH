#include<iostream.h>
#include<process.h>
void MoDH()
{

	int gdriver = DETECT, gmode, errorcode;


	initgraph(&gdriver, &gmode, "D:\\BC\\BGI");


	errorcode = graphresult();

	if (errorcode != grOk)
	{
		cout<<"Loi";
		 exit(1);
	}
}