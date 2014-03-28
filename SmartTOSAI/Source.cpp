#ifdef DBG
#include<conio.h>
#define pause() getch()
#else
#define pause()
#endif
#include<cstdio>
#include<iostream>
#include<Windows.h>
#include<easyx.h>
#include<cstring>
#include"src\board.h"
#include"src\Path.h"
#include"IDAstar.h"
#include<vector>
#include<cstdlib>
#include<ctime>
using namespace std;

#define BLUESTACK_TEXT "BlueStacks App Player for Windows (beta-1)"
//#define BLUESTACK_TEXT "¤pºâ½L"
HWND GetBluestackWindow()
{
	HWND hWnd;
	char text[101];
	do{
		hWnd=GetForegroundWindow();
		if(hWnd != NULL){
			GetWindowText(hWnd,text,100);
			if(strcmp(text,BLUESTACK_TEXT)==0){
				return hWnd;
			}
		}
		Sleep(500);
	}while(true);
	return NULL;
}


int main()
{
	srand(time(NULL));
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
	if(!SetWindowPos(hwndBluestack,HWND_TOPMOST,0,0,0,0,SWP_NOSIZE|SWP_NOMOVE))
	{
		cout<<"Error while SetWindowsPos:"<<GetLastError();pause();
		exit(1);
	}

	
	initgraph(600,600);
	while(true)
	{
		if(!CaptureWindowImage(hwndBluestack,&img))
		{
			cout<<"Some thing worng! Can\'t get screen!";
			pause();
			break;
		}
		cleardevice();
		putimage(0,0,&img);
		if(boardMain.loadFromImage(img))
		{
			vector<int> p;
			_Pos pos(3,3);
			for(int i=0;i<10;++i)
				p.push_back(rand()%8);
			applyPath(hwndBluestack,boardMain,p,pos);
		}
		Sleep(1000);
	}	
	return 0;
}
