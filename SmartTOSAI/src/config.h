#ifndef _CONFIG_H
#define _CONFIG_H

#include<fstream>

using std::fstream;
using std::endl;

static char ostr[][3]={"¤ô","¤õ","¤ì","¥ú","·t","¤ß"};

class config{

	int BoardTop;
	int BoardLeft;
	int BoardButtom;
	int BoardRight;
	int Speed;
	int Thread;
	int ThreadMothed;
public:
	int RequireCombo;
	int fiveUp;
	int antiCheat;
	int GetTop()const{return BoardTop;}
	int GetLeft()const{return BoardLeft;}
	int GetButtom()const{return BoardButtom;}
	int GetRight()const{return BoardRight;}
	int GetSpeed()const{return Speed;}
	int GetThread()const{return Thread;}
	int GetThreadMothed()const{return ThreadMothed;}
	friend bool LoadConfig(config &cfg);
	friend bool CreateConfig(config &cfg);
	friend bool SaveConfig(config &cfg);
};



#endif