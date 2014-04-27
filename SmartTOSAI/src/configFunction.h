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
	cout<<"�w��ϥ�SMART TOS AI"<<endl;
	cout<<"�즸�ϥνг]�w�^�������H�~�򥻵{��"<<endl;
	cout<<"���b���ԥؼ�...���I���z��bluestack��genymotion�����~��]�w"<<endl;
	
	HWND Bs=GetBluestackWindow();

	cout<<"�w�^���A�жi�J���@���d������N���~��"<<endl;
	_getch();

	FinishFlag=false;
	cfg.BoardLeft=0;
	cfg.BoardTop=250;
	EasyXWindow=initgraph(600,600);
	MessageBox(EasyXWindow,"Step1.�վ㥪�W���A�ϥ�ADWS���ʡA�ϥ�Q�O�s�]�w","SmartTOSAI",MB_ICONINFORMATION);
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
	MessageBox(EasyXWindow,"Step2.�վ�k�U���A�ϥ�ADWS���ʡA�ϥ�Q�O�s�]�w","SmartTOSAI",MB_ICONINFORMATION);
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
	cout<<"�P�±z�@�ߧ����]�w�A�N�~��SmartTOSAI"<<endl;
	Sleep(1000);
	system("CLS");
	return true;

}

#endif