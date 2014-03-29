#ifndef _IDASTAR_H
#define _IDASTAR_H

#ifdef EASYX
#undef EASYX
#endif


#include "src\board.h"
#include "src\Path.h"
#include <iostream>
#include <vector>
#include <algorithm>

#include<easyx.h>
using namespace std;

void display(Bead _B[8][8])
{
    cout<<endl;
    for(int i=1;i<=5;++i)
    {
        for(int j=1;j<=6;++j)
        {
            if(_B[i][j].color==0)cout<<"  ";
            else cout<<(int)_B[i][j].color<<' ';
        }
        cout<<endl;
    }
    cout<<endl;
    pause();
}

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

int clacComble(Bead _B[8][8])
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
                for(int k=0;k<Queue[j];++k){
                    flag[i][sum+k]=true;
					int rv=1;
					while(b[i][sum+k].color==b[i-rv][sum+k].color){
						legalComle&=!flag[i-rv][sum+k];
						rv++;
					}
                    //legalComle&= !( b[i][sum+k].color== b[i-1][sum+k].color && flag[i-1][sum+k]) ;
                }
                if(legalComle)
                    combo++;
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
                for(int k=0;k<Queue[i];++k)
                {
                    legalComle&= !flag[k+sum][j] && !(  b[k+sum][j].color==b[k+sum][j-1].color &&
                                                        flag[k+sum][j-1]);
                    flag[k+sum][j]=true;
                }
                if(legalComle)
                    combo++;
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

        combo+=clacComble(b);
    }
    return combo;
}

#define MAX_GOAL 7
#include<sstream>

std::ostringstream oss;
vector<int> * commonBest=nullptr;
_Pos nowpos,*resolvepos;
int coBest;

inline void updateBest(vector<int> *n,int comble)
{
	if(comble<0)comble=-comble;
    if(comble<coBest)return ;
    if(comble==coBest&&commonBest->size()<=n->size())return;
    (*commonBest)=(*n);
	coBest=comble;
	*resolvepos=nowpos;
	oss.str("");
	oss<<"THINK : "<<coBest<<" Combo in "<<commonBest->size()<<" Steps";
	outtextxy(0,0,oss.str().c_str());
}
int mdeep;
//if int<x : FIND GOAL
int LDFS(Board b,_Pos pos,int r,int deep,vector<int> *path)
{
    int comb=clacComble(b.b);
	updateBest(path,comb);
    int H=2*(MAX_GOAL-comb);
    if(H<=0)return -comb;
    if(H+deep>mdeep)return comb;
	if(userStop())return comb;
    _Pos fin;
    int cost;
    for(int i=0;i<4;++i)
    {
        if(i==r)continue;
        fin=pos;
        if(!fin.apply(i))continue;
        if(i>=4||b.b[fin.x][fin.y].color==b.b[pos.x][pos.y].color)cost=2;
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
void IDAStar(Board &b,vector<int> *path,_Pos *pos)
{
	userStopFlag=false;
    path->clear();
    commonBest=path;
	resolvepos=pos;
    coBest=0;
    vector<int> tmp;
    const int steplimit=30;
    for(mdeep=2;mdeep<steplimit;mdeep+=2)
    {
        for(int i=0;i<30;++i)
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