#ifdef DBG
#include<conio.h>
#define pause() getch()
#else
#define pause()
#endif
#define EASYX
#include<cstdio>
#include<iostream>
#include<Windows.h>
#include<easyx.h>
#include<cstring>
#include<vector>
#include<cstdlib>
#include<ctime>
#include"src\BluestackCapture.h"
#include"src\board.h"
#include"src\Path.h"

#include"IDAstar.h"
using namespace std;

int main()
{
	srand(time((time_t)NULL));
	HWND hwndBluestack;
	IMAGE img;
	Board boardMain;
	/*
	*	Intro Massage
	*/
	cout<<"SmartTOSAI By LFsWang!"<<endl
		<<"======================"<<endl
		<<"Ver."<<endl
		<<"Build :"<<__DATE__<<' '<<__TIME__<<endl
		<<"======================"<<endl<<endl;
	cout<<"Find Bluestack"<<endl
		<<"Please click your bluestack window to continue..."<<endl;

	/*
	*	Get BlueStack handle and set enviroment
	*/
	hwndBluestack=GetBluestackWindow();
	if(hwndBluestack==NULL)
	{
		cout<<"Error while handle Bluestack!"<<endl;pause();
		exit(1);
	}
	if(!SetWindowsTopMost(hwndBluestack))
	{
		cout<<"Error while SetWindowsPos:"<<GetLastError();pause();
		exit(1);
	}

	/*
	*	GUI Intro
	*/

	initgraph(600,600);
	while(true)
	{
		if(!CaptureWindowImage(hwndBluestack,&img))
		{
			cout<<"Some thing worng! Can\'t get screen!";
			break;
		}
		cleardevice();
		putimage(0,0,&img);
		if(loadFromImage(boardMain,img))
		{
			vector<int> p;
			_Pos pos(3,3);
			for(int i=0;i<50;++i)
				p.push_back(rand()%8);
			applyPath(hwndBluestack,boardMain,p,pos);
		}
		Sleep(1000);
	}
	pause();
	return 0;
}
