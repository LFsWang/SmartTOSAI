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

bool applyPath(HWND hWnd ,Board &bd,vector<int> &path,_Pos &pos,const config &cfg)
{
	ostringstream oss;
	RECT rectWin;
	int antiCheat;
	int winW,winH;
	int tmp=0;
	setlinestyle(PS_SOLID,6);
	setlinecolor(YELLOW);

	if(!GetClientRect(hWnd,&rectWin))
		return false;

	int TOP=cfg.GetTop();
	int LEFT=cfg.GetLeft();
	int RIGHT=cfg.GetRight();
	int DOWN=cfg.GetButtom();

	winW=RIGHT-LEFT;
	winH=DOWN-TOP;
#define ImgX(X) (winW*(2*(X)-1)/12)
#define ImgY(Y) (winH*(2*(Y)-1)/10)
#define RealX(X) (ImgX(X)+LEFT)
#define RealY(Y) (ImgY(Y)+TOP)

	antiCheat=cfg.antiCheat;
	SendMessage(hWnd,WM_LBUTTONDOWN,MK_LBUTTON,MAKELPARAM(RealX(pos.y),RealY(pos.x)));
	moveto(ImgX(pos.y),ImgY(pos.x));
	Sleep(100);
	int fixx,fixy;
	for(int &n:path)
	{
		tmp+=9;
		setlinecolor(YELLOW-tmp);
		fixx=rand()%5-1;
		fixy=rand()%5-1;
		pos.apply(n);
		if(n<4&&rand()%4==0&&antiCheat)
		{
			antiCheat--;
			for(int p=1;p<10;++p)
			{
				SendMessage(hWnd,WM_MOUSEMOVE,MK_LBUTTON,MAKELPARAM(RealX(pos.y)+rand()%5-3,RealY(pos.x)+rand()%5-3));
				Sleep(50);
			}
			SendMessage(hWnd,WM_MOUSEMOVE,MK_LBUTTON,MAKELPARAM(RealX(pos.y)+fixx,RealY(pos.x)+fixy));
			lineto(ImgX(pos.y)+fixx,ImgY(pos.x)+fixy);
		}
		else
		{
			SendMessage(hWnd,WM_MOUSEMOVE,MK_LBUTTON,MAKELPARAM(RealX(pos.y),RealY(pos.x)));
			lineto(ImgX(pos.y),ImgY(pos.x));
		}
		Sleep(cfg.GetSpeed());
	}
	MessageBeep(MB_OK);
	SendMessage(hWnd,WM_LBUTTONUP,MK_LBUTTON,MAKELPARAM(RealX(pos.y),RealY(pos.x)));
	return false;

}



#endif