#ifndef _PATH_H
#define _PATH_H
#include<vector>
#include<iostream>
#include<Windows.h>
#include"board.h"
#include<sstream>
#include<easyx.h>
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

bool applyPath(HWND hWnd ,Board &bd,vector<int> &path,_Pos &pos)
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
	winW=rectWin.right-rectWin.left;
	winH=(rectWin.bottom-rectWin.top)/2;


#define _X(X) (3+winW*(2*(X)-1)/12)
#define _Y(Y) (int)(  winH*(2*(Y)-1)/10.0+winH-40)
#define __Y(YY) (_Y(YY)-winH+40)
	antiCheat=3;
	SendMessage(hWnd,WM_LBUTTONDOWN,MK_LBUTTON,MAKELPARAM(_X(pos.y),_Y(pos.x)));
	//fillcircle(_X(pos.y),__Y(pos.x),15);
	moveto(_X(pos.y),__Y(pos.x));
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
				SendMessage(hWnd,WM_MOUSEMOVE,MK_LBUTTON,MAKELPARAM(_X(pos.y)+rand()%5-3,_Y(pos.x)+rand()%5-3));
				Sleep(50);
			}
			SendMessage(hWnd,WM_MOUSEMOVE,MK_LBUTTON,MAKELPARAM(_X(pos.y)+fixx,_Y(pos.x)+fixy));
			lineto(_X(pos.y)+fixx,__Y(pos.x)+fixy);
		}
		else
		{
			SendMessage(hWnd,WM_MOUSEMOVE,MK_LBUTTON,MAKELPARAM(_X(pos.y),_Y(pos.x)));
			lineto(_X(pos.y),__Y(pos.x));
		}
		Sleep(90);
	}
	MessageBeep(MB_OK);
	SendMessage(hWnd,WM_LBUTTONUP,MK_LBUTTON,MAKELPARAM(_X(pos.y),_Y(pos.x)));
	return false;
#undef _X
#undef _Y
#undef __Y
}



#endif