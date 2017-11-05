#ifndef MMALLOC_H
#define MMALLOC_H

#include <assert.h>
#include <iostream>

using namespace std;

#define LEN 1000
#define E 2	//分配空间的正误差（即分配空间最多比所需空间多E个字）
#define FootLoc(p) p+(p)->size-1	//指向p所指结点的底部

typedef struct Word	//Word:内存字类型
{
	union
	{
		Word * Llink;	//头部域，指向前驱结点
		Word * Uplink;	//底部域，指向本结点头部
	};
	int tag;	//块标志，0：空闲，1：占用，头部和尾部均有
	int size;	//头部域，块大小	
	Word *Rlink;	//头部域，指向后继结点
}Word,*PWord;

void space_init(PWord *spa,PWord mem);

//分配内存（首次拟合法）
void * mmalloc(PWord *spa,int len);

//回收内存
void mfree(PWord *spa,PWord *p);

//打印当前空闲内存块起止地址
void print_free(PWord av,PWord *spa);

//打印当前被占用的内存的地址
void print_used(PWord mem,PWord pav);

#endif