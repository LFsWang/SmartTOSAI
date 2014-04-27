#ifndef _CONFIGFUNCTION_H
#define _CONFIGFUNCTION_H

#include"config.h"
#include<iostream>
#include<fstream>
#include<Windows.h>
#include<conio.h>
#include<easyx.h>

using std::fstream;
using std::endl;
using std::cout;
using std::cin;

#define CONFIGFILEURL "cfg\\smartTosAi.cfg"

bool LoadConfig(config &cfg){
	fstream fin;

	fin.open(CONFIGFILEURL,std::ios::in);

	if(!fin){
		fin.close();
		return false;
	}
	//Screen Fix Setting
	fin>>cfg.BoardLeft>>cfg.BoardTop>>cfg.BoardRight>>cfg.BoardButtom;
	if(cfg.BoardLeft<0||cfg.BoardTop<0||cfg.BoardRight<0||cfg.BoardButtom<0)return false;
	if(cfg.BoardRight-cfg.BoardLeft<=0||cfg.BoardButtom-cfg.BoardTop<=0)return false;
	
	fin>>cfg.Speed;
	if(!fin)return false;
	if(cfg.Speed<90){
		cfg.Speed=90;
	}

	fin>>cfg.Thread>>cfg.ThreadMothed;
	if(!fin)return false;
	if(cfg.Thread<0||cfg.Thread>4){
		cfg.Thread=1;
	}
	cfg.RequireCombo=0;
	return true;
}

bool SaveConfig(config &cfg){
	fstream fout;

	fout.open(CONFIGFILEURL,std::ios::out);
	if(!fout){
		fout.close();
		return false;
	}
	//Screen Fix Setting
	fout<<cfg.BoardLeft<<' '<<cfg.BoardTop<<' '<<cfg.BoardRight<<' '<<cfg.BoardButtom<<endl;
	fout<<cfg.Speed<<endl;
	fout<<cfg.Thread<<' '<<cfg.ThreadMothed<<endl;
	return true;
}

bool CreateConfig(config &cfg){
	//Setting Screen
	bool FinishFlag;
	HWND EasyXWindow;
	IMAGE img;
	SetConsoleTitle("SMART TOS AI Init");
	cout<<"歡迎使用SMART TOS AI"<<endl;
	cout<<"初次使用請設定擷取視窗以繼續本程式"<<endl;
	cout<<"正在等候目標...請點擊您的bluestack或genymotion視窗繼續設定"<<endl;
	
	HWND Bs=GetBluestackWindow();

	cout<<"已擷取，請進入任一關卡後按任意鍵繼續"<<endl;
	_getch();

	FinishFlag=false;
	cfg.BoardLeft=0;
	cfg.BoardTop=250;
	EasyXWindow=initgraph(600,600);
	MessageBox(EasyXWindow,"Step1.調整左上角，使用ADWS移動，使用Q保存設定","SmartTOSAI",MB_ICONINFORMATION);
	while(!FinishFlag){
		cfg.BoardRight=cfg.BoardLeft+300;
		cfg.BoardButtom=cfg.BoardTop+300;
		if(!CaptureWindowImage(Bs,&img,cfg)){
			closegraph();
			return false;
		}
		putimage(0,0,&img);
		if(_kbhit()){
			char c=_getch();
			switch(c){
			case'A':
			case'a':
				cfg.BoardLeft--;
				break;
			case'D':
			case'd':
				cfg.BoardLeft++;
				break;
			case'W':
			case'w':
				cfg.BoardTop--;
				break;
			case'S':
			case's':
				cfg.BoardTop++;
				break;
			case'Q':
			case'q':
				FinishFlag=true;
				break;
			}
		}
		Sleep(10);
	}
	FinishFlag=false;
	MessageBox(EasyXWindow,"Step2.調整右下角，使用ADWS移動，使用Q保存設定","SmartTOSAI",MB_ICONINFORMATION);
	while(!FinishFlag){
		if(!CaptureWindowImage(Bs,&img,cfg)){
			closegraph();
			return false;
		}
		cleardevice();
		putimage(0,0,&img);
		if(_kbhit()){
			char c=_getch();
			switch(c){
			case'A':
			case'a':
				if(cfg.BoardLeft<cfg.BoardRight-1)
					cfg.BoardRight--;
				break;
			case'D':
			case'd':
				cfg.BoardRight++;
				break;
			case'W':
			case'w':
				if(cfg.BoardTop<cfg.BoardButtom-1)
				cfg.BoardButtom--;
				break;
			case'S':
			case's':
				cfg.BoardButtom++;
				break;
			case'Q':
			case'q':
				FinishFlag=true;
				break;
			}
		}
		Sleep(10);
	}
	closegraph();

	cfg.Speed=120;
	cfg.RequireCombo=0;
	cfg.Thread=2;
	cfg.ThreadMothed=1;

	cout<<"Save..."<<endl;
	if(!SaveConfig(cfg)){
		return false;
	}
	cout<<"感謝您耐心完成設定，將繼續SmartTOSAI"<<endl;
	Sleep(1000);
	system("CLS");
	return true;

}

#endif