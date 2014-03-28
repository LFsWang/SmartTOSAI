#ifndef _BLUESTACKCAPTURE_H
#define _BLUESTACKCAPTURE_H
#include<Windows.h>
#include<Easyx.h>
HWND GetBluestackWindow();
bool CaptureWindowImage(HWND hWnd,IMAGE *img);


bool CaptureWindowImage(HWND hWnd,IMAGE *img)
{
	RECT 	rectWindow;
	HDC 	hdcScreen;
	HDC		hdcImg;
	int W,H;

	if(!GetWindowRect(hWnd,&rectWindow))
		{goto err;}
	W=rectWindow.right-rectWindow.left;
	H=(rectWindow.bottom-rectWindow.top)/2;

	hdcScreen	=GetDC(NULL);
	hdcImg		=GetImageHDC(img);

	Resize(img,W,H);
	if(!BitBlt(hdcImg,0,0,W,H,hdcScreen,rectWindow.left,rectWindow.top+H-40,SRCCOPY))
	{goto err;}
	
	ReleaseDC(hWnd,hdcScreen);
	ReleaseDC(GetHWnd(),hdcImg);

	return true;
err:
	cerr<<GetLastError()<<endl;
	return false;
}
#endif