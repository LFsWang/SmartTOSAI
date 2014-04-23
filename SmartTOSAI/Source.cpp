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
#include<direct.h>

#include<thread>
#include<future>
#include<chrono>

#include"src\BluestackCapture.h"
#include"src\board.h"
#include"src\Path.h"
#include"src\configFunction.h"
#include"IDAstar.h"
using namespace std;

int main(int argv,char *argc[])
{
	if(argv==2&&strcmp(argc[1],"whoami?")==0){
		system("start http://forum.tfcis.org/?Sylveon=LFsWang");
	}
	srand(time(nullptr));
	HWND hwndBluestack;
	IMAGE img;
	Board boardMain;
	ostringstream oss;
	config globalConfig;
	char buffer[100];
	int aiTImeLimit=16;
	/*
	Init
	*/
	_mkdir("img");
	if(!BoardInit()){
		cout<<"color.dat遺失或損毀"<<endl;
		_getch();
		exit(0);
	}
	if(!LoadConfig(globalConfig)){
		if(!CreateConfig(globalConfig)){
			cout<<"Config ERROR!"<<endl;
			_getch();
			exit(0);
		}
	}
	/*
	*	Load AI use C++ 11
	*/
	
	/*
	*	Intro Massage
	*/
	cout<<"SmartTOSAI By LFsWang!"<<endl
		<<"======================"<<endl
		<<"Ver.0.31alpha"<<endl
		<<"Build :"<<__DATE__<<' '<<__TIME__<<endl
		<<"======================"<<endl<<endl;
	cout<<"Set AI Timelimit (Second) :";
	cin>>aiTImeLimit;

	if(!cin||aiTImeLimit<1){
		cin.clear();
		while(cin.get()!='\n');
		cout<<"setting error. use default 16sec"<<endl;
		aiTImeLimit=16;
	}

	cout<<"Find Bluestack"<<endl
		<<"Please click your TOS window to continue..."<<endl;

	/*
	*	Get BlueStack handle and set enviroment
	*/
	hwndBluestack=GetBluestackWindow();
	if(hwndBluestack==NULL)
	{
		cout<<"Error while handle Screen!"<<endl;pause();
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
	initgraph(	globalConfig.GetRight()-globalConfig.GetLeft(),
				globalConfig.GetButtom()-globalConfig.GetTop()+120);

	LOGFONT fontStyle;
	gettextstyle(&fontStyle);
	strcpy_s(fontStyle.lfFaceName,"consolas");
	fontStyle.lfQuality=ANTIALIASED_QUALITY;
	settextstyle(&fontStyle);

	globalConfig.antiCheat=3;
	while(true)
	{
		if(Stopflag||GetAsyncKeyState(VK_ESCAPE)){
			while(true){
				outtextxy(0,0,"Locked! Press F1 to continue...");
				if(GetAsyncKeyState(VK_F1)){
					break;
				}
				if(GetAsyncKeyState(VK_F2)){
					if(globalConfig.antiCheat)globalConfig.antiCheat=0;
					else globalConfig.antiCheat=3;
					oss.str("");
					oss<<"ANTI"<<globalConfig.antiCheat;
					outtextxy(0,15,oss.str().c_str());
					Sleep(100);
				}
				if(GetAsyncKeyState(VK_F8)){
					BoardInit();
					Sleep(10);
				}
				if(GetAsyncKeyState(VK_F5)){
					InputBox(buffer,99,"請輸入色珠的限制，大寫為必須轉出，小寫為優先轉出\n火F水W木G光L暗D心H");
					int result=0,tmp;
					for(int i=0;buffer[i];++i){
						switch(buffer[i]){
						case 'W':result|=0x000002;
						case 'w':result|=0x000001;break;
						case 'F':result|=0x000020;
						case 'f':result|=0x000010;break;
						case 'G':result|=0x000200;
						case 'g':result|=0x000100;break;
						case 'L':result|=0x002000;
						case 'l':result|=0x001000;break;
						case 'D':result|=0x020000;
						case 'd':result|=0x010000;break;
						case 'H':result|=0x200000;
						case 'h':result|=0x100000;break;
						}
					}
					tmp=result;
					oss.str("");
					oss<<"轉珠限制：";
					for(int i=0;i<6;++i){
						int k=tmp%16;
						tmp/=16;
						if(k){
							
							if(k&0x2)
								oss<<"強迫";
							oss<<ostr[i]<<',';
						}
					}
					if(MessageBox(GetHWnd(),oss.str().c_str(),"Sure?",MB_YESNO|MB_ICONQUESTION)==IDYES){
						globalConfig.RequireCombo=result;
					}
				}
				Sleep(10);
			}
			Stopflag=false;
		}
		if(!CaptureWindowImage(hwndBluestack,&img,globalConfig))
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
			future<void> task=async(IDAStar,boardMain,&path,&posStart,&globalConfig,0);
			Board tt=boardMain;
			vector<int> tp;
			_Pos pp;
			//future<void> taska=async(IDAStar,tt,&tp,&pp,&globalConfig,1);
			double runtime;
			clock_start=clock();
			do{
				taskStatus = task.wait_for(std::chrono::milliseconds(100));
				clock_now=clock();
				runtime=((double)clock_now-clock_start)/CLOCKS_PER_SEC;

				sprintf_s(buffer,"已思考 %.2f 秒",runtime);
				outtextxy(0,15,buffer);

				if(GetAsyncKeyState(VK_ESCAPE)||runtime>aiTImeLimit){
					outtextxy(0,15,"Send Stop Signal..");
					CloseHandle(hMutex);
					hMutex=NULL;
					if(runtime<=aiTImeLimit){
						Stopflag=true;
					}
				}
			}while(taskStatus != future_status::ready);
			if(Stopflag){
				continue;
			}
			
			if(hMutex!=NULL){
				CloseHandle(hMutex);
			}
			task.get();
			//taska.get();
			if(path.size()==0){
				MessageBox(GetHWnd(),"Fail to find Path!","Error",MB_ICONERROR);
			}
			
			outtextxy(0,0,"Finish!");

			if(!path.empty()){
				applyPath(hwndBluestack,boardMain,path,posStart,globalConfig);
				//oss.str("");
				//oss<<"img\\"<<time(NULL)<<".bmp";
				//saveimage(oss.str().c_str());
				Sleep(3000);
			}
		}
		Sleep(1000);
	}
	pause();
	return 0;
}
