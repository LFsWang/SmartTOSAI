#ifndef _PATH_H
#define _PATH_H
#include<vector>
#include<iostream>
#include<Windows.h>
#include"board.h"
#include<sstream>
#include<easyx.h>
#include"config.h"

using std::ostringstream;
using std::vector;


static int const dx[8]={ 0, 0, 1,-1, 1, 1,-1,-1};
static int const dy[8]={ 1,-1, 0, 0, 1,-1, 1,-1};
static int const rv[8]={ 1, 0, 3, 2, 7, 6, 5, 4};

//extern _Move Move;
class _Pos{
public:
	int x;
	int y;
	_Pos(){}
	_Pos(int _x,int _y):x(_x),y(_y){}
	bool apply(int d)
	{
		if(x+dx[d]<=0||5<x+dx[d])
			return false;
		if(y+dy[d]<=0||6<y+dy[d])
			return false;
		x+=dx[d];
		y+=dy[d];
		return true;
	}
};
//Global
int TOP;
int LEFT;
int RIGHT;
int DOWN;
int winW,winH;

#define ImgX(X) (winW*(2*(X)-1)/12)
#define ImgY(Y) (winH*(2*(Y)-1)/10)
#define RealX(X) (ImgX(X)+LEFT)
#define RealY(Y) (ImgY(Y)+TOP)

//Init
bool PathInit(config &cfg)
{
	TOP=cfg.GetTop();
	LEFT=cfg.GetLeft();
	RIGHT=cfg.GetRight();
	DOWN=cfg.GetButtom();
	winW=RIGHT-LEFT;
	winH=DOWN-TOP;
	return true;
}
//move function
inline void mouseDown(HWND hWnd,int x,int y)
{
	SendMessage(hWnd,WM_LBUTTONDOWN,MK_LBUTTON,MAKELPARAM(x,y));
	moveto(x-LEFT,y-TOP);
	fillcircle(x-LEFT,y-TOP,5);
}
inline void mouseRelease(HWND hWnd,int x,int y)
{
	SendMessage(hWnd,WM_LBUTTONUP,MK_LBUTTON,MAKELPARAM(x,y));
	fillcircle(x-LEFT,y-TOP,5);
}
inline void mouseMove(HWND hWnd,int x,int y)
{
	SendMessage(hWnd,WM_MOUSEMOVE,MK_LBUTTON,MAKELPARAM(x,y));
	lineto(x-LEFT,y-TOP);
}
bool applyPath(HWND hWnd ,Board &bd,vector<int> &path,_Pos &pos,const config &cfg)
{
	int antiCheat;
	int tmp=0;
	int fixx,fixy;

	setlinestyle(PS_SOLID,6);
	setlinecolor(YELLOW);

	antiCheat=cfg.antiCheat;

	mouseDown(hWnd,RealX(pos.y),RealY(pos.x));
	Sleep(100);
	moveto(ImgX(pos.y),ImgY(pos.x));
	Sleep(100);

	
	for(int &n:path)
	{
		tmp+=9;
		setlinecolor(YELLOW-tmp);
		fixx=rand()%9-4;
		fixy=rand()%9-4;
		pos.apply(n);
		if(n<4&&rand()%4==0&&antiCheat)
		{
			antiCheat--;
			for(int p=1;p<7;++p)
			{
				mouseMove(hWnd,RealX(pos.y)+rand()%5-3,RealY(pos.x)+rand()%5-3);
				Sleep(60+rand()%40);
			}
			mouseMove(hWnd,RealX(pos.y)+fixx,RealY(pos.x)+fixy);
		}
		else
		{
			mouseMove(hWnd,RealX(pos.y),RealY(pos.x));
		}
		Sleep(cfg.GetSpeed()+rand()%10);
	}
	mouseRelease(hWnd,RealX(pos.y),RealY(pos.x));
	MessageBeep(MB_OK);
	return true;
}
#endif