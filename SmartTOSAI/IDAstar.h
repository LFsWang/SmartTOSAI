#ifndef _IDASTAR_H
#define _IDASTAR_H

#ifdef EASYX
#undef EASYX
#endif
#define THREADMAX 4
#define ddbg MessageBox(NULL,"","",MB_OK)
#include "src\board.h"
#include "src\Path.h"
#include "src\config.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <sstream>
#include <cstring>
#include <easyx.h>

using namespace std;

vector<int> threadDo[THREADMAX];

struct theadInfo{
	config cfg;
	//IDA info
	int maxgoal;
	_Pos nowpos;
	char buffer[90];
	ostringstream oss;
	unsigned int comboInfo;
	int MaxDeep;
};

theadInfo tcfg[THREADMAX];

struct bestInfo{
	int numMaxCombo;
	int ComboInfo;
	int numBestMatch;
	int isFive;
	vector<int> *path;
	_Pos *start;
};

bestInfo tinfo[THREADMAX];

int uid=0;

inline bool userStop(){
	HANDLE hMutex=OpenMutex(MUTEX_ALL_ACCESS,false,"_Smart_User_Wait_Flag");
	if(hMutex==NULL){
		return true;
	}else{
		CloseHandle(hMutex);
		return false;
	}
}


#define plusCombo(C) (tcfg[tid].comboInfo+=(1<<(C-1)*4))
int calcCombo(Bead _B[8][8],int tid)
{
	Bead b[8][8];
	bool flag[8][8];
	int Queue[8];
	int size,sum,combo=0,allcombo=0;
	bool legalComle;
	for(int i=0;i<8;++i)
		for(int j=0;j<8;++j)
			b[i][j]=_B[i][j];
	do{
		combo=0;
		memset(flag,0,sizeof(flag));
		//row
		for(int i=1;i<=5;++i)
		{
			size=0;
			for(int j=1;j<=6;++j)
			{
				if( b[i][j].color==C_EMPTY || b[i][j].color!=b[i][j-1].color){
					Queue[size++]=1;
				}else{
					Queue[size-1]++;
				}
			}
			sum=1;
			for(int j=0;j<size;++j)
			{
				legalComle=true;
				if(Queue[j]>=3)
				{
					if(Queue[j]>=5)
						tinfo[tid].isFive=true;
					//if(b[i][sum].color==C_HEART)heartFlag=true;
					for(int k=0;k<Queue[j];++k){
						flag[i][sum+k]=true;
						int rv=1;
						while(b[i][sum+k].color==b[i-rv][sum+k].color){
							legalComle&=!flag[i-rv][sum+k];
							rv++;
						}
						//legalComle&= !( b[i][sum+k].color== b[i-1][sum+k].color && flag[i-1][sum+k]) ;
					}
					if(legalComle){
						plusCombo(b[i][sum].color);
						combo++;
					}else{
						tinfo[tid].isFive=true;
					}
				}
				sum+=Queue[j];
			}
		}
		for(int j=1;j<=6;++j)
		{
			size=0;
			for(int i=1;i<=5;++i)
			{
				if(b[i][j].color==C_EMPTY || b[i][j].color!=b[i-1][j].color){
					Queue[size++]=1;
				}else{
					Queue[size-1]++;
				}
			}
			sum=1;
			for(int i=0;i<size;++i)
			{
				if(Queue[i]>=3)
				{
					if(Queue[i]>=5)
						tinfo[tid].isFive=true;
					legalComle=true;

					//if(b[sum][j].color==C_HEART)heartFlag=true;
					for(int k=0;k<Queue[i];++k)
					{
						legalComle&= !flag[k+sum][j] && !(  b[k+sum][j].color==b[k+sum][j-1].color &&
							flag[k+sum][j-1]);
						flag[k+sum][j]=true;
					}
					if(legalComle){
						plusCombo(b[sum][j].color);
						combo++;
						if(Queue[i]>=5)
							tinfo[tid].isFive=true;
					}else{
						tinfo[tid].isFive=true;
					}
				}
				sum+=Queue[i];
			}
		}
		if(combo!=0){
			for(int i=1;i<=6;++i)
			{
				int ptr=5;
				for(int j=5;j>0&&b[j][i].color!=C_EMPTY;--j)
				{
					if(!flag[j][i])
					{
						b[ptr--][i]=b[j][i];
					}
				}
				while(ptr>0)
					b[ptr--][i].color=C_EMPTY;
			}
			allcombo+=combo;
			//combo+=calcCombo(b,tid);
		}
	}while(combo!=0);
	return allcombo;
}



inline void updateBest(vector<int> *n,int comble,int tid,Board &b)
{
	if(comble<0)comble=-comble;

	ostringstream &oss=tcfg[tid].oss;

	int delta=tcfg[tid].cfg.GetButtom()-tcfg[tid].cfg.GetTop()+tid*30;

	bool isOK=true;

	int req=tcfg[tid].cfg.RequireCombo;
	int now=tcfg[tid].comboInfo;
	int nreq=0;

	for(int i=0;i<6;++i){
		int _r=req%16;
		int _n=now%16;
		if(_r){
			if(_n)nreq++;
			else{
				if(_r&0x2){
					isOK=false;
				}
			}
		}
		req/=16;
		now/=16;
	}

	if(!isOK){
		return ;
	}
	if(tcfg[tid].cfg.fiveUp){
		nreq;
	}
	if(tcfg[tid].cfg.fiveUp && tinfo[tid].isFive){
		nreq+=6;
	}
	//BAD
	if(nreq<tinfo[tid].numBestMatch){
		return ;
	}
	else if(nreq==tinfo[tid].numBestMatch){
		//BAD
		if(comble<tinfo[tid].numMaxCombo)return ;
		//BAD
		if(comble==tinfo[tid].numMaxCombo){
			if(tinfo[tid].path->size() <= n->size())return ;
			//if(commonBest->size()<=n->size())return ;
		}
	}

	//BAD
	(*tinfo[tid].path)=(*n);
	tinfo[tid].numMaxCombo=comble;
	tinfo[tid].ComboInfo=tcfg[tid].comboInfo;
	tinfo[tid].numBestMatch=nreq;
	*tinfo[tid].start=tcfg[tid].nowpos;
	//*resolvepos=nowpos;

	sprintf_s(tcfg[tid].buffer,"預計最大combo:%2d,AI%2d 找到 %2d combo (%2d Steps,Goal%2d)",tcfg[tid].maxgoal,tid,tinfo[tid].numMaxCombo,tinfo[tid].path->size(),tinfo[tid].numBestMatch);
	outtextxy(0,delta,tcfg[tid].buffer);

	oss.str("");
	oss<<"當前解:";

	int tmp=tinfo[tid].ComboInfo;
	for(int i=0;i<6;++i){
		int p=tmp%16;
		tmp/=16;
		oss<<p<<ostr[i];
	}
	//if(tinfo[tid].isFive)oss<<"DD";
	//else oss<<"ss";
	outtextxy(0,delta+15,oss.str().c_str());
}
//int mdeep;
//if int<x : FIND GOAL
int LDFS(Board &b,_Pos &pos,int r,int deep,vector<int> *path,int tid)
{
	_Pos fin;
	int cost,H;

	tcfg[tid].comboInfo=0;
	tinfo[tid].isFive=false;
	int comb=calcCombo(b.b,tid);

	updateBest(path,comb,tid,b);

	H=2*(tcfg[tid].maxgoal-comb);

	if(H<=0)		return -comb;
	if(H+deep>tcfg[tid].MaxDeep)return comb;
	if(userStop())	return comb;

	for(int i=0;i<8;++i)
	{
		if(i==r)continue;
		fin=pos;
		if(!fin.apply(i))continue;

		if(b.b[fin.x][fin.y].color==b.b[pos.x][pos.y].color)cost=2;
		//else if(i>=4&&deep<15)cost=2;
		else cost=1;

		path->push_back(i);
		swap(b.b[fin.x][fin.y],b.b[pos.x][pos.y]);

		int rt=LDFS(b,fin,rv[i],deep+cost,path,tid);
		if(rt<0){
			return rt;
		}

		swap(b.b[fin.x][fin.y],b.b[pos.x][pos.y]);
		path->pop_back();
	}
	return comb;
}
//OK
int clacMaxGoal(const Board &b){
	int c[7]={0};
	for(int i=1;i<=5;++i)
		for(int j=1;j<=6;++j)
			c[b.b[i][j].color]++;
	int all=0;
	for(int i=1;i<=6;++i){
		all+=c[i]/3;
	}
	return all;
}

int IDAStar(Board &b,vector<int> *path,_Pos *pos,config *_cfg,int tid)
{

	tcfg[tid].maxgoal=clacMaxGoal(b);

	tcfg[tid].cfg=*_cfg;


	path->clear();
	tinfo[tid].path=path;
	tinfo[tid].start=pos;

	tinfo[tid].ComboInfo=0;
	tinfo[tid].numBestMatch=0;
	tinfo[tid].numMaxCombo=0;
	/*TEST*/

	vector<int> tmp;
	const int steplimit=60;
	int &deep=tcfg[tid].MaxDeep;
	for(deep=3;deep<steplimit;deep+=3)
	{
		for(int &i : threadDo[tid]){
			//tcfg[tid].nowpos=_Pos(1+i/6,1+i%5);
			tcfg[tid].nowpos=_Pos(1+i/6,1+i%6);
			tmp.clear();
			int rt=LDFS(b,tcfg[tid].nowpos,-1,0,&tmp,tid);
			if(rt<0||userStop()){	
				goto FINISH;
			}
		}
	}
FINISH:;
	return tinfo[tid].numMaxCombo;
}

bool AIInit(config &cfg){
	char buf[128];
	ifstream fin;
	stringstream ss;
	int tmp;
	int getUid=++uid;
	

	sprintf_s(buf,"cfg\\%dt%d.txt",cfg.GetThread(),cfg.GetThreadMothed());
	fin.open(buf);
	if(!fin)return false;

	for(int i=0;i<cfg.GetThread();++i){
		threadDo[i].clear();
		fin.getline(buf,128);
		ss.clear();
		ss<<buf;
		while(ss>>tmp){
			if(0<=tmp&&tmp<30)
				threadDo[i].push_back(tmp);
		}
	}
	return true;

}

#endif