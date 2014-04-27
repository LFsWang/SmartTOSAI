#ifdef DBG
#include<conio.h>
#define pause() getch()
#else
#define pause()
#endif

#define EASYX
#define _USE_MATH_DEFINES

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
		initgraph(245,350);
		loadimage(NULL,"IMAGE",MAKEINTRESOURCE(130));
		_getch();
		closegraph();
	}

	srand(time(nullptr));
	HWND hwndBluestack;
	IMAGE img;
	Board boardMain;
	ostringstream oss;
	config globalConfig;
	char buffer[100];
	int aiTImeLimit=16;
	int Threadnum=1;
	/*
	Init
	*/
	_mkdir("cfg");
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

	if(!AIInit(globalConfig)){
		cout<<"配置檔 cfg\\"<<globalConfig.GetThread()<<'t'<<globalConfig.GetThreadMothed()<<".txt 遺失"<<endl;
		_getch();
		exit(0);
	}
	/*
	*	Load AI use C++ 11
	*/
	
	/*
	*	Intro Massage
	*/
	cout<<"         SmartTOSAI By LFsWang!    "<<endl
		<<"==================================="<<endl
		<<"      Ver.0.4alpha    "<<endl
		<<"Build  :"<<__DATE__<<' '<<__TIME__<<endl
		<<"Thread :"<<globalConfig.GetThread()<<endl
		<<"Method :"<<globalConfig.GetThreadMothed()<<endl
		<<endl
		<<"       本作品由LFsWang開發製作"<<endl
		<<"   授權方法：CC - 姓名標示─非商業性"<<endl
		<<"    開發時須保留whoami?及本頁資訊"<<endl
		<<"==================================="<<endl<<endl;
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
				globalConfig.GetButtom()-globalConfig.GetTop()+45*globalConfig.GetThread());

	LOGFONT fontStyle;
	gettextstyle(&fontStyle);
	strcpy_s(fontStyle.lfFaceName,"consolas");
	fontStyle.lfQuality=ANTIALIASED_QUALITY;
	settextstyle(&fontStyle);

	globalConfig.antiCheat=3;
	globalConfig.fiveUp=false;
	Threadnum=globalConfig.GetThread();

	while(true)
	{
		if(Stopflag||GetAsyncKeyState(VK_ESCAPE)){
			while(true){
				outtextxy(0,0,"Locked! Press F1 to continue... (F5 setting)");
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
					InputBox(buffer,99,"請輸入色珠的限制，大寫為必須轉出，小寫為優先轉出\n火F水W木G光L暗D心H\n5可全體珠(暫時無法指定哪一色)");
					int result=0,tmp;
					globalConfig.fiveUp=false;
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
						case '5':globalConfig.fiveUp=true;break;
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
					if(globalConfig.fiveUp){
						oss<<"5連珠";
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
		if(loadFromImage(boardMain,img,globalConfig))
		{
			outtextxy(0,0,"THINK...");

			vector<int> path[THREADMAX];
			_Pos posStart[THREADMAX];
			Board AIboard[THREADMAX];
			future<int> task[THREADMAX];
			future_status::future_status taskStatus;

			HANDLE hMutex;
			time_t clock_start;
			time_t clock_now;
			int flag=0;

			hMutex=CreateMutex(NULL,false,"_Smart_User_Wait_Flag");
			if(GetLastError() == ERROR_ALREADY_EXISTS){
				MessageBox(GetHWnd(),"Mutex is used!\nPlease check if anything use!","ERROR_ALREADY_EXISTS",MB_ICONERROR);
				continue ;
			}

			//future<int> task=async(IDAStar,boardMain,&path,&posStart,&globalConfig,0);
			for(int i=0;i<Threadnum;++i){
				AIboard[i]=boardMain;
				task[i]=async(IDAStar,AIboard[i],&path[i],&posStart[i],&globalConfig,i);
			}

			double runtime;
			clock_start=clock();
			do{
				flag=0;
				for(int i=0;i<Threadnum;++i){
					taskStatus = task[i].wait_for(std::chrono::milliseconds(0));
					if(taskStatus == future_status::ready){
						flag=1;
					}
				}
				
				clock_now=clock();
				runtime=((double)clock_now-clock_start)/CLOCKS_PER_SEC;
				sprintf_s(buffer,"已思考 %.2f 秒",runtime);
				outtextxy(0,15,buffer);

				if(runtime>aiTImeLimit){
					outtextxy(0,15,"Send Stop Signal..");
					flag=1;
				}
				if(GetAsyncKeyState(VK_ESCAPE)){
					outtextxy(0,15,"Send Stop Signal..");
					flag=-1;
					Stopflag=true;
				}
				Sleep(50);
			}while(flag==0);

			if(hMutex!=NULL){
				CloseHandle(hMutex);
				hMutex=NULL;
			}

			if(Stopflag){
				continue;
			}
			
			int tmpc,MaxC=task[0].get(),use=0;
			for(int i=1;i<Threadnum;++i){
				tmpc=task[i].get();
				if(tmpc>MaxC || tmpc==MaxC && path[0].size()>path[i].size()){
					path[0]=path[i];
					posStart[0]=posStart[i];
					use=i;
				}
			}
			
			if(path[0].size()==0){
				MessageBox(GetHWnd(),"Fail to find Path!","Error",MB_ICONERROR);
				continue;
			}

			oss.str("");
			oss<<"Finish! use AI "<<use<<endl;
			outtextxy(0,0,oss.str().c_str());

			if(!path[0].empty()){
				applyPath(hwndBluestack,boardMain,path[0],posStart[0],globalConfig);
				Sleep(5000);
			}
		}
		Sleep(1000);
	}
	pause();
	return 0;
}
