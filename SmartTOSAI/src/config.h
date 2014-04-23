#ifndef _CONFIG_H
#define _CONFIG_H

#include"board.h"
#include"BluestackCapture.h"
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
public:
	int RequireCombo;
	int antiCheat;
	int GetTop()const{return BoardTop;}
	int GetLeft()const{return BoardLeft;}
	int GetButtom()const{return BoardButtom;}
	int GetRight()const{return BoardRight;}
	int GetSpeed()const{return Speed;}
	friend bool LoadConfig(config &cfg);
	friend bool CreateConfig(config &cfg);
	friend bool SaveConfig(config &cfg);
};


#endif