#ifndef _BOARD_H
#define _BOARD_H


#include<sstream>
#include<iomanip>
#include<vector>
#include<Windows.h>
#include<fstream>
#include<vector>
/*for debug*/
using std::ostringstream;

#define C_EMPTY	0
#define C_WATER	1
#define C_FILE	2
#define C_WOOD	3
#define C_RAY	4
#define C_DARK	5
#define C_HEART 6

#define T_ERROR		-1
#define T_NORMAL	0
#define T_STRENGTH	1
#define T_INHIBIT	2

struct Bead{
	BYTE color;
	BYTE type;
	Bead():color(0),type(0){}
	Bead(BYTE c,BYTE t):color(c),type(t){}
};

class Board{
public:
	Bead b[8][8];
};


#ifdef EASYX
#include<easyx.h>
bool loadFromImage(Board &bo,IMAGE &img)
{
	
	static int dx[]={0,7,7,-7,-7,10,-10,0,0};
	static int dy[]={0,7,-7,7,-7,0,0,10,-10};

	ostringstream oss;
	int winW,winH;
	BYTE c,t;
	float H,S,V,h,s,v;
	
	bool flag=true;

	winW=img.getwidth()-8;
	winH=img.getheight()+5;

	for(int i=6;i>0;--i)
	{
		for(int j=1;j<=5;++j)
		{
			int x=4+winW*(2*i-1)/12;
			int y=  winH*(2*j-1)/10-3;
			/*
			*	may be it can delete max & min
			*/
			H=0;S=0;V=0;
			for(int k=0;k<9;++k)
			{
				COLORREF c=getpixel(x+dx[k],y+dy[k]);
				RGBtoHSV(c,&h,&s,&v);
				H+=h;S+=s;V+=v;
			}
			H/=9;S/=9;V/=9;

			if(H<20&&S>0.89){c=C_FILE;t=T_NORMAL;}//RED OK
			else if((10<H&&H<32||i==0&&j==4&&80<H&&H<83) &&V>0.94){c=C_FILE;t=T_STRENGTH;}
			else if(108<H&&H<144&&V<0.9){c=C_WOOD;t=T_NORMAL;}//GREEN
			else if(108<H&&H<144&&V>0.9){c=C_WOOD;t=T_STRENGTH;}
			else if(40 <H&&H<100&&V<0.9){c=C_RAY;t=T_NORMAL;}//YELLOW
			else if(38 <H&&H<100&&V>0.9){c=C_RAY;t=T_STRENGTH;}
			else if(325<H&&H<350&&V<0.9){c=C_HEART;t=T_NORMAL;}//HEART
			else if(325<H&&H<350&&V>0.9){c=C_HEART;t=T_STRENGTH;}
			else if(190<H&&H<210&&V<0.9){c=C_WATER;t=T_NORMAL;}//Blue
			else if(150<H&&H<210&&V>0.9){c=C_WATER;t=T_STRENGTH;}
			else if(290<H&&H<310&&V<0.9){c=C_DARK;t=T_NORMAL;}//Dark
			else if(250<H&&H<295&&V>0.9){c=C_DARK;t=T_STRENGTH;}
			else{
				//Debug info
				//setfillcolor(RED);
				//fillcircle(x,y,10);
				//Debug info
				oss.str("");
				oss<<H<<' '<<S<<' '<<V;
				outtextxy(x,y+15*(i-3),oss.str().c_str());

				c=C_EMPTY;
				t=T_NORMAL;
				flag=false;
			}
			bo.b[j][i].color=c;
			bo.b[j][i].type=t;
		}
	}
	return flag;
}
#endif /*EASYX*/

#endif /*_BOARD_H*/