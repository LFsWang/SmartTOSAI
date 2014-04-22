#ifndef _IDASTAR_H
#define _IDASTAR_H

#ifdef EASYX
#undef EASYX
#endif


#include "src\board.h"
#include "src\Path.h"
#include "src\config.h"
#include <iostream>
#include <vector>
#include <algorithm>
#include<sstream>

#include<easyx.h>
using namespace std;

//#define MAX_GOAL 8
int maxgoal;
config *cfg;
ostringstream oss;


bool userStopFlag;
inline bool userStop(){
	if(userStopFlag)return true;
	
	HANDLE hMutex=OpenMutex(MUTEX_ALL_ACCESS,false,"_Smart_User_Wait_Flag");
	if(hMutex==NULL){
		userStopFlag=true;
		return true;
	}else{
		CloseHandle(hMutex);
		return false;
	}
}

bool heartFlag;
unsigned int comboInfo;
#define plusCombo(C) (comboInfo+=(1<<(C-1)*4))
int calcCombo(Bead _B[8][8])
{
    Bead b[8][8];
    bool flag[8][8];
    int Queue[8];
    int size,sum,combo=0;
    bool legalComle;
    //memcpy(&b,&_B,sizeof(64)*sizeof(Bead));
	for(int i=0;i<8;++i)
		for(int j=0;j<8;++j)
			b[i][j]=_B[i][j];
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

        combo+=calcCombo(b);
    }
    return combo;
}


vector<int> * commonBest=nullptr;
_Pos nowpos,* resolvepos=nullptr;
int coBest;
int coBestinfo;
int coBestReq;
bool HeartForce=false;

inline void updateBest(vector<int> *n,int comble)
{
	int delta=cfg->GetButtom()-cfg->GetTop();

	bool isOK=true;
	
	int req=cfg->RequireCombo;
	int now=comboInfo;
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

	if(nreq<coBestReq){
		return ;
	}
	
	if(comble<0)comble=-comble;

    if(comble<coBest)return ;

    if(comble==coBest){
		if(commonBest->size()<=n->size())return ;
	}

    (*commonBest)=(*n);
	coBest=comble;
	coBestinfo=comboInfo;
	coBestReq=nreq;
	*resolvepos=nowpos;

	oss.str("");
	oss<<"預計最大combo:"<<maxgoal<<" AI : found "<<coBest<<" Combo in "<<commonBest->size()<<" Steps  ";
	outtextxy(0,delta,oss.str().c_str());

	oss.str("");
	oss<<"當前解:";
	int tmp=coBestinfo;
	for(int i=0;i<6;++i){
		int p=tmp%16;
		tmp/=16;
		oss<<p<<ostr[i];
	}
	outtextxy(0,delta+15,oss.str().c_str());

	moveto(0,delta+45);
	outtext("要求解:");
	
	tmp=cfg->RequireCombo;
	for(int i=0;i<6;++i){
		int p=tmp%16;
		tmp/=16;

		oss.str("");
		if(p&0x2){
			settextcolor(WHITE);
			oss<<'*';
		}
		else if(p&0x1){
			if((coBestinfo>>4*i)&0xF)
				settextcolor(WHITE);
			else
				settextcolor(LIGHTRED);
			oss<<'+';
		}
		else{
			settextcolor(BLACK);
			oss<<'-';
		}
		oss<<ostr[i];
		outtext(oss.str().c_str());
	}
	settextcolor(WHITE);
}
int mdeep;
//if int<x : FIND GOAL
int LDFS(Board &b,_Pos &pos,int r,int deep,vector<int> *path)
{
	_Pos fin;
    int cost,H;

	heartFlag=false;
	comboInfo=0;
    int comb=calcCombo(b.b);

	updateBest(path,comb);

    H=2*(maxgoal-comb);

    if(H<=0)		return -comb;
    if(H+deep>mdeep)return comb;
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

        int rt=LDFS(b,fin,rv[i],deep+cost,path);
        if(rt<0){
                return rt;
        }

        swap(b.b[fin.x][fin.y],b.b[pos.x][pos.y]);
        path->pop_back();
    }
    return comb;
}

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

void IDAStar(Board &b,vector<int> *path,_Pos *pos,config *_cfg)
{
	maxgoal=clacMaxGoal(b);
	userStopFlag=false;
	
	cfg=_cfg;

    path->clear();
    commonBest=path;
	resolvepos=pos;

	coBestinfo=0;
    coBest=0;
	coBestReq=0;
	/*TEST*/

    vector<int> tmp;
    const int steplimit=40;
    for(mdeep=3;mdeep<steplimit;mdeep+=3)
    {
        for(int i=29;i>=0;--i)
        {
            nowpos=_Pos(1+i/6,1+i%5);
            tmp.clear();
            int rt=LDFS(b,nowpos,-1,0,&tmp);
            if(rt<0||userStop()){
                goto FINISH;
            }
        }
    }
    FINISH:;
}



#endif