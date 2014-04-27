#ifndef _BOARD_H
#define _BOARD_H



#include<Windows.h>
#include<sstream>
#include<iomanip>
#include<fstream>

#include<cstdio>
#include<vector>
#include<cmath>

#include"config.h"

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
double cH[12][2];

bool BoardInit(){
	std::ifstream fin("cfg\\color.dat");
	if(!fin)return false;
	for(int i=0;i<12;++i){
		for(int j=0;j<2;++j){
			fin>>cH[i][j];
			if(!fin)return false;
			if(cH[i][j]<0||cH[i][j]>360)return false;
		}
	}
	fin.close();
	return true;
}

void GetPointColorXY(int rx,int ry,config &cfg,float &h,float &s,float &v){
	int winW=cfg.GetRight()-cfg.GetLeft();
	int winH=cfg.GetButtom()-cfg.GetTop();
	int x=winW*(2*rx-1)/12;
	int y=winH*(2*ry-1)/10;
	int dx,dy;
	double r=(winW/12.0)*0.25;
	float _H,_S,_V;
	const int samplenum=60;
	h=s=v=0;
	COLORREF color;
	for(int i=0;i<samplenum;++i){
		dx=r*std::cos(M_PI*2*i/samplenum);
		dy=r*std::sin(M_PI*2*i/samplenum);
		color=getpixel(x+dx,y+dy);
		RGBtoHSV(color,&_H,&_S,&_V);
		h+=_H;s+=_S;v+=_V;
		putpixel(x+dx,y+dy,LIGHTRED);
	}
	h/=samplenum;s/=samplenum;v/=samplenum;
}

bool loadFromImage(Board &bo,IMAGE &img,config &cfg)
{
	char ostr[][3]={"","水","火","木","光","暗","心"};
	char tstr[][3]={"","強","風"};
	static int dx[]={0,7,7,-7,-7,10,-10,0,0};
	static int dy[]={0,7,-7,7,-7,0,0,10,-10};

	ostringstream oss;
	char strBuffer[100];
	int winW,winH;
	BYTE c,t;
	float H,S,V;
	
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
			GetPointColorXY(i,j,cfg,H,S,V);
			//test
			if     (cH[0][0] <H&&H<cH[0][1] &&V<0.9){c=C_FILE;t=T_NORMAL;}//RED OK
			else if(cH[1][0] <H&&H<cH[1][1] &&V>0.9){c=C_FILE;t=T_STRENGTH;}
			else if(cH[2][0] <H&&H<cH[2][1] &&V<0.9){c=C_WOOD;t=T_NORMAL;}//GREEN
			else if(cH[3][0] <H&&H<cH[3][1] &&V>0.9){c=C_WOOD;t=T_STRENGTH;}
			else if(cH[4][0] <H&&H<cH[4][1] &&V<0.9){c=C_RAY;t=T_NORMAL;}//YELLOW 41 1 0.7
			else if(cH[5][0] <H&&H<cH[5][1] &&V>0.9){c=C_RAY;t=T_STRENGTH;}//38~108
			else if(cH[6][0] <H&&H<cH[6][1] &&V<0.9){c=C_HEART;t=T_NORMAL;}//HEART
			else if(cH[7][0] <H&&H<cH[7][1] &&V>0.9){c=C_HEART;t=T_STRENGTH;}
			else if(cH[8][0] <H&&H<cH[8][1] &&V<0.9){c=C_WATER;t=T_NORMAL;}//Blue
			else if(cH[9][0] <H&&H<cH[9][1] &&V>0.9&&S>0.35){c=C_WATER;t=T_STRENGTH;}
			else if(cH[10][0]<H&&H<cH[10][1]&&V<0.9){c=C_DARK;t=T_NORMAL;}//Dark 298 0.9 0.6
			else if(cH[11][0]<H&&H<cH[11][1]&&V>0.9){c=C_DARK;t=T_STRENGTH;}//290~255 0.36 1
			else{
				flag=false;
				c=C_EMPTY;
				t=T_NORMAL;
			}


			if(c==C_EMPTY){
				sprintf_s(strBuffer,"%.1f %.2f",H,S);
			}else{
				sprintf_s(strBuffer,"%s%s",tstr[t],ostr[c]);
			}
			outtextxy(x,y,strBuffer);	

			bo.b[j][i].color=c;
			bo.b[j][i].type=t;
		}
	}
	return flag;
}
#endif /*EASYX*/

#endif /*_BOARD_H*/