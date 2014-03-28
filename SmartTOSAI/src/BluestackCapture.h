#ifndef _BLUESTACKCAPTURE_H
#define _BLUESTACKCAPTURE_H
#include<Windows.h>

#define BLUESTACK_TEXT "BlueStacks App Player for Windows (beta-1)"
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
inline bool SetWindowsTopMost(HWND hWnd)
{
	return SetWindowPos(hWnd,HWND_TOPMOST,0,0,0,0,SWP_NOSIZE|SWP_NOMOVE);
}
#ifdef EASYX
#include<Easyx.h>
bool CaptureWindowImage(HWND hWnd,IMAGE *img)
{
	RECT 	rectWindow;
	HDC 	hdcScreen;
	HDC		hdcImg;
	int W,H;

	if(!GetWindowRect(hWnd,&rectWindow))
		{return false;}
	W=rectWindow.right-rectWindow.left;
	H=(rectWindow.bottom-rectWindow.top)/2;

	hdcScreen	=GetDC(NULL);
	hdcImg		=GetImageHDC(img);

	Resize(img,W,H);
	if(!BitBlt(hdcImg,0,0,W,H,hdcScreen,rectWindow.left,rectWindow.top+H-40,SRCCOPY))
	{return false;}
	
	ReleaseDC(hWnd,hdcScreen);
	ReleaseDC(GetHWnd(),hdcImg);

	return true;
}
#endif
#endif