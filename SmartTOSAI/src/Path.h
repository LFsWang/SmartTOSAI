#ifndef _PATH_H
#define _PATH_H
#include<vector>
#include<iostream>
#include<Windows.h>
#include"board.h"
#include<sstream>
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
		if(x+dx[d]<0||5<=x+dx[d])
			return false;
		if(y+dy[d]<0||6<=y+dy[d])
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
	int winW,winH;
	if(!GetClientRect(hWnd,&rectWin))
		return false;
	winW=rectWin.right-rectWin.left;
	winH=(rectWin.bottom-rectWin.top)/2;
#define _X(X) (3+winW*(2*(X)+1)/12)
#define _Y(Y) (  winH*(2*(Y)+1)/10+winH-40)
#define __Y(YY) (_Y(YY)-winH+40)
	setfillcolor(LIGHTRED);
	SendMessage(hWnd,WM_LBUTTONDOWN,MK_LBUTTON,MAKELPARAM(_X(pos.y),_Y(pos.x)));
	fillcircle(_X(pos.y),__Y(pos.x),15);
	Sleep(100);
	for(int &n:path)
	{
		pos.apply(n);
		fillcircle(_X(pos.y),__Y(pos.x),15);
		SendMessage(hWnd,WM_MOUSEMOVE,MK_LBUTTON,MAKELPARAM(_X(pos.y),_Y(pos.x)));
		//oss.str("");
		//oss<<pos.y<<' '<<pos.x<<' '<<_X(pos.y)<<' '<<__Y(pos.x);
		//MessageBox(GetHWnd(),oss.str().c_str(),"DEGUG",MB_OK);
		
		Sleep(100);
	}
	MessageBeep(MB_OK);
	SendMessage(hWnd,WM_LBUTTONUP,MK_LBUTTON,MAKELPARAM(_X(pos.y),_Y(pos.x)));
	return false;
#undef _X
#undef _Y
#undef __Y
}



#endif