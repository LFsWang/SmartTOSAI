#ifndef _CONFIG_H
#define _CONFIG_H

#include"src\board.h"

class config{
	int AppointColor;

public:
	int GetAppointColor(){
		return AppointColor;
	}
	int GetAppointColor(int color){
		return (AppointColor>>color)&1;
	}
	int SetAppointColor(int color,int type){
		type<<=color;
	}
};

#endif