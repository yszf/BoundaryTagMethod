#include "mmalloc.h"


//初始化一块空闲内存块
void space_init(PWord *spa,PWord mem)
{
	assert(mem != NULL);

	PWord pav = &mem[0];
	mem[0].Llink = pav;
	mem[0].tag = 0;
	mem[0].size = LEN;
	mem[0].Rlink = pav;
	mem[LEN-1].Uplink = pav;
	mem[LEN-1].tag = 0;
	
	*spa = pav;
}

//分配内存（首次拟合法）
void * mmalloc(PWord *spa,int len)//请求分配的字节
{
	//判断还有没有内存
	if(spa == NULL)
	{
		return NULL;
	}
	int n;
	n = (len+sizeof(Word)-1)/sizeof(Word)+2;//计算所请求的字块数
	PWord p,f;
	for (p=*spa;p&&p->size<n && p->Rlink != *spa;p=p->Rlink)
		;	//查找不小于size的空闲块
	if (!p || p->size<n)//找不到合适的空闲块
	{
		return NULL;
	}
	else //寻找到不小于n的空闲块
	{
		f = FootLoc(p);	//指向此空闲块的底部
		*spa = p->Rlink; //spa指向p结点的后继结点
		if(p->size-n<=E) //整块分配，忽略<=E的剩余量
		{
			if (*spa == p) //当前只有一个块
			{
				*spa = NULL;	//被占用后可利用表变为空表
			}
			else //在表中删除分配的结点
			{
				(*spa)->Llink = p->Llink;
				p->Llink->Rlink = *spa;
			}
			p->tag = f->tag = 1;
		}
		else
		{
			f->tag = 1;	//修改分配块的底部标志
			p->size -= n; //置剩余块大小

			f = FootLoc(p); //指向剩余块底部
			f->tag = 0;	//设置剩余块底部标志
			f->Uplink = p;

			p = f + 1;
			p->tag = 1;
			p->size = n;
		}
	}
	return p;//p结点的llink和rlink域并未改变，这对回收空间有用

}

//回收内存
void mfree(PWord *spa,PWord *p)//p指向要释放的内存块
{
	assert(*p != NULL);

	PWord s,t,q;
	int l,r;//l,r为指示释放块的左右邻块是否空闲
	l = (*p-1)->tag;
	r = (*p+(*p)->size)->tag;

	if (!(*spa))	//可利用空间表为空
	{
		*spa = (*p)->Llink = (*p)->Rlink = *p;//修改空闲表指针
		(*p)->tag = (FootLoc(*p))->tag = 0;//修改头尾标志
		(FootLoc(*p))->Uplink = *p;
	}
	else	//可利用空间表不为空
	{
		if (l && r) //左右邻区均不为空
		{
			(*p)->tag = 0;
			(FootLoc(*p))->Uplink = *p;
			(FootLoc(*p))->tag = 0;

			q = (*spa)->Llink;
			(*p)->Rlink = *spa;
			(*p)->Llink = q;
			q->Rlink = (*spa)->Llink = *p;
			*spa = *p;	//令刚释放的结点为下次分配是的最先查询的结点	
		}
		else if (!l && r)//左邻区为空，右邻区不为空
		{
			(*p)->tag = 0;
			s = (*p-1)->Uplink;//s指向左邻区(空闲时)的首地址
			s->size += (*p)->size;//设置新空闲块的大小
			(FootLoc(*p))->Uplink = s;//设置新的空闲块的的底部
			(FootLoc(*p))->tag = 0;
		}
		else if (l && !r)//左邻区不为空，右邻区为空
		{
			t = *p + (*p)->size; //t指向右邻区(空闲时)的首地址
			(*p)->tag = 0;//p为合并后的结点的头部首地址
			(FootLoc(*p))->tag = 0;

			q = t->Llink;//此处q为右邻区的前驱结点首地址
			(*p)->Llink = q;//链接q与p
			q->Rlink = *p; 

			q = t->Rlink; //此处q为右邻区的后继结点的首地址
			(*p)->Rlink = q; //链接q与p
			q->Llink = *p;

			(*p)->size += t->size; //新空闲块大小
			(FootLoc(t))->Uplink = *p;//底部指针指向新结点的头部

			if (*spa == t)
			{
				*spa = *p;
			}
		}
		else
		{
			s = (*p-1)->Uplink;//s指向做邻区(空闲时)的首地址
			t = *p + (*p)->size; //t指向右邻区(空闲时)的首地址
			
			(*p)->tag = 0;
			(FootLoc(*p))->tag = 0; 
			s->size += (*p)->size+t->size;//设置新结点的大小
			t->Llink->Rlink = t->Rlink;//跨过t链接
			t->Rlink->Llink = t->Llink;
			(FootLoc(t))->Uplink = s;	//新结点底部指针指向其头部

			if (*spa == t)
			{
				*spa = s;
			}
		}
	}
	*p = NULL;
}

//打印当前空闲内存块起止地址
void print_free(PWord av,PWord *spa)
{
	assert(spa != NULL);
	PWord p = *spa;
	cout<<"空闲块起止范围：";
	cout<<p-av+1<<"<--->"<<p+p->size-av<<" | ";
	while (p->Rlink != *spa)
	{
		p = p->Rlink;
		cout<<p-av+1<<"<--->"<<p+p->size-av<<" | ";
	}
	cout<<endl;
	

}

//打印当前被占用的内存的地址
void print_used(PWord mem,PWord pav)
{
	PWord r;
	r = pav;
	cout<<"非空闲块起止范围："<<endl;

	for (int i=0;i<LEN;++i)
	{
		if (r->tag == 1)
		{
			cout<<r-mem+1<<"<--->";
			r++;
			continue;
		}

		if (r != mem && r->tag != 0 && (r-1)->tag == 1)
		{
			while(r->tag != 1)
			{
				r++;
			}
			cout<<r-mem+1<<" | ";

			if (r-mem+1 == LEN)
			{
				break;
			}
		}
		r++;
	}
	cout<<endl;

}