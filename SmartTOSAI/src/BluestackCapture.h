#ifndef _BLUESTACKCAPTURE_H
#define _BLUESTACKCAPTURE_H

#include<Windows.h>
#include"config.h"

#define BLUESTACK_TEXT "BlueStacks App Player for Windows (beta-1)"
HWND GetBluestackWindow()
{
	HWND hWnd;
	char text[101];
	do{
		hWnd=GetForegroundWindow();
		if(hWnd != NULL){
			GetWindowText(hWnd,text,100);
			if(strncmp(text,BLUESTACK_TEXT,20)==0){
				return hWnd;
			}
			if(text[0]=='G'&&text[1]=='e'){
				return hWnd;
			}
		}
		Sleep(50);
	}while(true);
	return NULL;
}

inline bool SetWindowsTopMost(HWND hWnd)
{
	return SetWindowPos(hWnd,HWND_TOPMOST,0,0,0,0,SWP_NOSIZE|SWP_NOMOVE);
}

#ifdef EASYX
#include<Easyx.h>
bool CaptureWindowImage(HWND hWnd,IMAGE *img,const config &cfg)
{
	RECT 	rectWindow;
	HDC 	hdcScreen;
	HDC		hdcImg;
	int W,H;

	if(!GetWindowRect(hWnd,&rectWindow))
		{return false;}
	int TOP=cfg.GetTop();
	int LEFT=cfg.GetLeft();
	int RIGHT=cfg.GetRight();
	int DOWN=cfg.GetButtom();
	W=RIGHT-LEFT;
	H=DOWN-TOP;

	hdcScreen	=GetDC(NULL);
	hdcImg		=GetImageHDC(img);

	Resize(img,W,H);
	if(!BitBlt(hdcImg,0,0,W,H,hdcScreen,rectWindow.left+LEFT,rectWindow.top+TOP,SRCCOPY))
	{return false;}
	
	ReleaseDC(hWnd,hdcScreen);
	ReleaseDC(GetHWnd(),hdcImg);

	return true;
}
#endif
#endif