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