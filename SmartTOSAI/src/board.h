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
#include<fstream>
double cH[12][2];

bool BoardInit(){
	std::ifstream fin("color.dat");
	if(!fin)return false;
	for(int i=0;i<12;++i){
		for(int j=0;j<2;++j){
			fin>>cH[i][j];
			if(!fin)return false;
			if(cH[i][j]<0||cH[i][j]>360)return false;
		}
	}
	return true;
}

bool loadFromImage(Board &bo,IMAGE &img)
{
	
	static int dx[]={0,7,7,-7,-7,10,-10,0,0};
	static int dy[]={0,7,-7,7,-7,0,0,10,-10};

	ostringstream oss;
	char strBuffer[100];
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

			H=0;S=0;V=0;
			for(int k=0;k<9;++k)
			{
				COLORREF c=getpixel(x+dx[k],y+dy[k]);
				RGBtoHSV(c,&h,&s,&v);
				H+=h;S+=s;V+=v;
			}
			H/=9;S/=9;V/=9;
			bool dbg=false;
			if     (cH[0][0] <H&&H<cH[0][1] &&V<0.9){c=C_FILE;t=T_NORMAL;}//RED OK
			else if(cH[1][0] <H&&H<cH[1][1] &&V>0.9){c=C_FILE;t=T_STRENGTH;}
			else if(cH[2][0] <H&&H<cH[2][1] &&V<0.9){c=C_WOOD;t=T_NORMAL;}//GREEN
			else if(cH[3][0] <H&&H<cH[3][1] &&V>0.9){c=C_WOOD;t=T_STRENGTH;}
			else if(cH[4][0] <H&&H<cH[4][1] &&V<0.9){c=C_RAY;t=T_NORMAL;}//YELLOW 41 1 0.7
			else if(cH[5][0] <H&&H<cH[5][1] &&V>0.9){c=C_RAY;t=T_STRENGTH;}//38~108
			else if(cH[6][0] <H&&H<cH[6][1] &&V<0.9){c=C_HEART;t=T_NORMAL;}//HEART
			else if(cH[7][0] <H&&H<cH[7][1] &&V>0.9){c=C_HEART;t=T_STRENGTH;}
			else if(cH[8][0] <H&&H<cH[8][1] &&V<0.9){c=C_WATER;t=T_NORMAL;}//Blue
			else if(cH[9][0] <H&&H<cH[9][1] &&V>0.9){c=C_WATER;t=T_STRENGTH;}
			else if(cH[10][0]<H&&H<cH[10][1]&&V<0.9){c=C_DARK;t=T_NORMAL;}//Dark 298 0.9 0.6
			else if(cH[11][0]<H&&H<cH[11][1]&&V>0.9){c=C_DARK;t=T_STRENGTH;}//290~255 0.36 1
			else{
				dbg=true;
				flag=false;
				c=C_EMPTY;
				t=T_NORMAL;
			}
			if(dbg||t==T_STRENGTH){
				//Debug info
				oss.str("");
				sprintf_s(strBuffer,"%.2f %.2f %.2f",H,S,V);
				oss<<strBuffer;
				outtextxy(x,y+15*(i-3),oss.str().c_str());				
			}
			bo.b[j][i].color=c;
			bo.b[j][i].type=t;
		}
	}
	return flag;
}
#endif /*EASYX*/

#endif /*_BOARD_H*/