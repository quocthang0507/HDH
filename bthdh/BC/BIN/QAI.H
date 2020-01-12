#ifndef __qai_h



#define __qai_h

#ifndef __STDIO_H
#include <stdio.h>
#endif

#ifndef __CONIO_H
#include <conio.h>
#endif

#ifndef __DOS_H
#include <dos.h>
#endif


#ifndef __STDLIB_H
#include <stdlib.h>
#endif

#ifndef __GRAPHICS_H
#include <graphics.h>
#endif

#ifndef __STRING_H
#include <string.h>
#endif

#ifndef __PROCESS_H
#include <process.h>
#endif


class dohoa
{
private:

public:

};

void ktdh(void)
{
	int gdriver = DETECT, gmode, errorcode;
	initgraph(&gdriver, &gmode, "D:\\BC\\BGI");
	errorcode = graphresult();
	if (errorcode != grOk)
	{
		printf("Graphics error: %s\n", grapherrormsg(errorcode));
		printf("Press any key to halt:");
		getch();
		exit(1);             /* return with error code */
	}
}


void dda(int x1,int y1,int x2,int y2,int color=YELLOW)
{
	float x=x1,y=y1,temp=(abs(x2-x1)>=abs(y2-y1))?abs(x2-x1):abs(y2-y1);
	putpixel(int(x+.05),int(y+0.5),color);
	for(int i=0;i<temp;i++)
	{
		x+=(x2-x1)/temp;
		y+=(y2-y1)/temp;
		putpixel(int(x+.05),int(y+0.5),color);
	}
}
void put8pixel(int x,int y,int xc,int yc,int c)
{
	putpixel( x+xc , y+yc, c);
	putpixel(-x+xc , y+yc, c);
	putpixel( x+xc ,-y+yc, c);
	putpixel(-x+xc ,-y+yc, c);
	putpixel( y+xc , x+yc, c);
	putpixel(-y+xc , x+yc, c);
	putpixel( y+xc ,-x+yc, c);
	putpixel(-y+xc ,-x+yc, c);
}

void hinhtron(int xc,int yc,int r,int c=8)
{
	int x=0,y=r,p=3-2*r;
	put8pixel(x,y,xc,yc,c);
	while (x<=y)
	{
		if(p<0)p+=4*x+++6;
		else p+= 4*(x++-y--)+10;
		put8pixel(x,y,xc,yc,c);
	}
}
void totrai(int,int,int,int);
void tophai(int,int,int,int);
void totren(int,int,int,int);
void toduoi(int,int,int,int);
void totrai(int x,int y,int mauto,int maubien)
{
	int maudiem;
	maudiem=getpixel(x,y);
	if((maudiem!=mauto)&&(maudiem!=maubien))
	{
		putpixel(x,y,mauto);
		totrai(x-1,y,mauto,maubien);
		totren(x,y+1,mauto,maubien);
		toduoi(x,y-1,mauto,maubien);
	}
}
void tophai(int x,int y,int mauto,int maubien)
{
	int maudiem;
	maudiem=getpixel(x,y);
	if((maudiem!=mauto)&&(maudiem!=maubien))
	{
		putpixel(x,y,mauto);
		tophai(x+1,y,mauto,maubien);
		totren(x,y+1,mauto,maubien);
		toduoi(x,y-1,mauto,maubien);
	}
}
void totren(int x,int y,int mauto,int maubien)
{
	int maudiem;
	maudiem=getpixel(x,y);
	if((maudiem!=mauto)&&(maudiem!=maubien))
	{
		putpixel(x,y,mauto);
		totrai(x-1,y,mauto,maubien);
		tophai(x+1,y,mauto,maubien);
		totren(x,y+1,mauto,maubien);
	}
}
void toduoi(int x,int y,int mauto,int maubien)
{
	int maudiem;
	maudiem=getpixel(x,y);
	if((maudiem!=mauto)&&(maudiem!=maubien))
	{
		putpixel(x,y,mauto);
		totrai(x-1,y,mauto,maubien);
		tophai(x+1,y,mauto,maubien);
		toduoi(x,y-1,mauto,maubien);
	}
}
void tomau(int x,int y,int mauto,int maubien)
{
	int maudiem;
	maudiem=getpixel(x,y);
	if((maudiem!=mauto)&&(maudiem!=maubien))
	{
		putpixel(x,y,mauto);
		totrai(x-1,y,mauto,maubien);
		tophai(x+1,y,mauto,maubien);
		totren(x,y+1,mauto,maubien);
		toduoi(x,y-1,mauto,maubien);
	}
}
void hcn(int x1,int y1,int x2,int y2,int color)// hinh chu nhat thiet lap 1 hinh chu nhat nhu ham window
{
	for(int i=y1;i<=y2;i++)
		for(int j=x1;j<=x2;j++)
			putpixel(j,i,color);
}

void xvm(unsigned char* a,int x,int y,int c = WHITE) //xeng viet mang 16 int
{
	int i,j;
	for(i=0;i<16;i++)
	{
		for(j=0;j<8;j++)
		{
			if(a[i]&1)putpixel(x+8-j,y,c);
			a[i]>>=1;
		}
		y++;
	}
}

void xvietf(unsigned char *xau,int x,int y,int c=WHITE)
{
	int temp=strlen(xau);
	unsigned char a[16];
	FILE *p=fopen("quang.x","rb");
	for(int i=0;i<temp;i++)
	{
		fseek(p,((unsigned char)xau[i])*16,SEEK_SET);
		fread(a,sizeof(char),16,p);
		xvm(a,x,y,c);
		x+=10;
	}
	fclose(p);
}

void xviet(char ch,int x,int y,int c=8)
{
	unsigned char a[2];
	a[0]=(unsigned char)(ch);
	a[1]=NULL;
	xvietf(a,x,y,c);
}

void xviet(char *xau,int x,int y,int c=8)
{
	unsigned char *temp;
	int n=strlen(xau);
	temp=(unsigned char *)malloc(n);
	for(int i=0;i<=n;i++)
		temp[i]=(unsigned char)(xau[i]);
	xvietf(temp,x,y,c);
	free(temp);
}
void background()
{
	int i,j;
	char str[5];
	//chia vach + chu so truc tung
	for(i=10,j=9;i<370;i+=40,j--)
	{
		itoa(j,str,10);
		dda(10,i+10,15,i+10,8);
		xviet(str,0,i+2,8);
	}
	//goc toa do
	xviet('0',0,i+12,8);
	//duong thang len  , truc tung
	dda(20,10,20,380,8);
	dda(20,10,15,15,8);
	dda(20,10,25,15,8);

	//duong thang ngang , truc hoanh
	dda(20,380,610,380,8);
	dda(610,380,605,375,8);
	dda(610,380,605,385,8);

	//vach truc hoanh
	for(i=60,j=1;i<610;i+=40,j++)
	{
		dda(i,385,i,390,8);
		itoa(j,str,10);
		if (j<10) xviet(str,i-4,392,8);
		else xviet(str,i-8,392,8);
	}

	//dot matrix
	for(i=20;i<620;i+=10)
		for(j=10;j<380;j+=10)
			putpixel(i,j,8);
}












#endif