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

#include<thread>
#include<future>
#include<chrono>

#include"src\BluestackCapture.h"
#include"src\board.h"
#include"src\Path.h"

#include"IDAstar.h"
using namespace std;

int main()
{
	srand(time(nullptr));
	HWND hwndBluestack;
	IMAGE img;
	Board boardMain;
	ostringstream oss;
	/*
	*	Load AI use C++ 11
	*/
	/*
	*	Intro Massage
	*/
	cout<<"SmartTOSAI By LFsWang!"<<endl
		<<"======================"<<endl
		<<"Ver.0.21alpha"<<endl
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
	bool Stopflag=false;
	initgraph(600,600);
	Sleep(3000);
	int config[2]={0};/*TEST*/
	while(true)
	{
		if(Stopflag||GetAsyncKeyState(VK_ESCAPE)){
			while(true){
				outtextxy(0,0,"Locked! Press F1 to continue...");
				if(GetAsyncKeyState(VK_F1)){
					break;
				}
				if(GetAsyncKeyState(VK_F5)){
					config[0]=(config[0]+1)%2;
					Sleep(300);
				}
				if(config[0]){
					outtextxy(0,50,"Force Heart ON ");
				}else{
					outtextxy(0,50,"Force Heart OFF");
				}
				Sleep(10);
			}
			Stopflag=false;
		}
		if(!CaptureWindowImage(hwndBluestack,&img))
		{
			cout<<"Some thing worng! Can\'t get screen!";
			break;
		}
		cleardevice();
		putimage(0,0,&img);
		if(loadFromImage(boardMain,img))
		{
			vector<int> path;
			_Pos posStart;
			HANDLE hMutex;
			time_t clock_start;
			time_t clock_now;
			int flag=0;
			outtextxy(0,0,"THINK...");

			hMutex=CreateMutex(NULL,false,"_Smart_User_Wait_Flag");
			if(GetLastError() == ERROR_ALREADY_EXISTS){
				MessageBox(GetHWnd(),"Mutex is used!\nPlease check if anything use!","ERROR_ALREADY_EXISTS",MB_ICONERROR);
				continue ;
			}

			future_status::future_status taskStatus;
			future<void> task=async(IDAStar,boardMain,&path,&posStart,config);
			double time;
			clock_start=clock();
			do{
				taskStatus = task.wait_for(std::chrono::milliseconds(100));
				clock_now=clock();
				time=((double)clock_now-clock_start)/CLOCKS_PER_SEC;

				oss.str("");
				oss<<time<<"Second";
				if(config[0]){
					oss<<"ForceHeart : ON";
				}

				outtextxy(0,15,oss.str().c_str());
				if(GetAsyncKeyState(VK_ESCAPE)||time>20){
					outtextxy(0,15,"Send Stop Signal..");
					CloseHandle(hMutex);
					hMutex=NULL;
					if(time<=20){
						Stopflag=true;
					}
				}
			}while(taskStatus != future_status::ready);
			if(Stopflag){
				continue;
			}
			task.get();
			if(hMutex!=NULL){
				//ReleaseMutex(hMutex);
				CloseHandle(hMutex);
			}
			if(path.size()==0){
				MessageBox(GetHWnd(),"Fail to find Path!","Error",MB_ICONERROR);
			}
			//IDAStar(boardMain,&p,&posStart);
			outtextxy(0,0,"OK...");
			applyPath(hwndBluestack,boardMain,path,posStart);
			Sleep(3000);
		}
		Sleep(1000);
	}
	pause();
	return 0;
}
