#include "mmalloc.h"


//��ʼ��һ������ڴ��
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

//�����ڴ棨�״���Ϸ���
void * mmalloc(PWord *spa,int len)//���������ֽ�
{
	//�жϻ���û���ڴ�
	if(spa == NULL)
	{
		return NULL;
	}
	int n;
	n = (len+sizeof(Word)-1)/sizeof(Word)+2;//������������ֿ���
	PWord p,f;
	for (p=*spa;p&&p->size<n && p->Rlink != *spa;p=p->Rlink)
		;	//���Ҳ�С��size�Ŀ��п�
	if (!p || p->size<n)//�Ҳ������ʵĿ��п�
	{
		return NULL;
	}
	else //Ѱ�ҵ���С��n�Ŀ��п�
	{
		f = FootLoc(p);	//ָ��˿��п�ĵײ�
		*spa = p->Rlink; //spaָ��p���ĺ�̽��
		if(p->size-n<=E) //������䣬����<=E��ʣ����
		{
			if (*spa == p) //��ǰֻ��һ����
			{
				*spa = NULL;	//��ռ�ú�����ñ��Ϊ�ձ�
			}
			else //�ڱ���ɾ������Ľ��
			{
				(*spa)->Llink = p->Llink;
				p->Llink->Rlink = *spa;
			}
			p->tag = f->tag = 1;
		}
		else
		{
			f->tag = 1;	//�޸ķ����ĵײ���־
			p->size -= n; //��ʣ����С

			f = FootLoc(p); //ָ��ʣ���ײ�
			f->tag = 0;	//����ʣ���ײ���־
			f->Uplink = p;

			p = f + 1;
			p->tag = 1;
			p->size = n;
		}
	}
	return p;//p����llink��rlink��δ�ı䣬��Ի��տռ�����

}

//�����ڴ�
void mfree(PWord *spa,PWord *p)//pָ��Ҫ�ͷŵ��ڴ��
{
	assert(*p != NULL);

	PWord s,t,q;
	int l,r;//l,rΪָʾ�ͷſ�������ڿ��Ƿ����
	l = (*p-1)->tag;
	r = (*p+(*p)->size)->tag;

	if (!(*spa))	//�����ÿռ��Ϊ��
	{
		*spa = (*p)->Llink = (*p)->Rlink = *p;//�޸Ŀ��б�ָ��
		(*p)->tag = (FootLoc(*p))->tag = 0;//�޸�ͷβ��־
		(FootLoc(*p))->Uplink = *p;
	}
	else	//�����ÿռ��Ϊ��
	{
		if (l && r) //������������Ϊ��
		{
			(*p)->tag = 0;
			(FootLoc(*p))->Uplink = *p;
			(FootLoc(*p))->tag = 0;

			q = (*spa)->Llink;
			(*p)->Rlink = *spa;
			(*p)->Llink = q;
			q->Rlink = (*spa)->Llink = *p;
			*spa = *p;	//����ͷŵĽ��Ϊ�´η����ǵ����Ȳ�ѯ�Ľ��	
		}
		else if (!l && r)//������Ϊ�գ���������Ϊ��
		{
			(*p)->tag = 0;
			s = (*p-1)->Uplink;//sָ��������(����ʱ)���׵�ַ
			s->size += (*p)->size;//�����¿��п�Ĵ�С
			(FootLoc(*p))->Uplink = s;//�����µĿ��п�ĵĵײ�
			(FootLoc(*p))->tag = 0;
		}
		else if (l && !r)//��������Ϊ�գ�������Ϊ��
		{
			t = *p + (*p)->size; //tָ��������(����ʱ)���׵�ַ
			(*p)->tag = 0;//pΪ�ϲ���Ľ���ͷ���׵�ַ
			(FootLoc(*p))->tag = 0;

			q = t->Llink;//�˴�qΪ��������ǰ������׵�ַ
			(*p)->Llink = q;//����q��p
			q->Rlink = *p; 

			q = t->Rlink; //�˴�qΪ�������ĺ�̽����׵�ַ
			(*p)->Rlink = q; //����q��p
			q->Llink = *p;

			(*p)->size += t->size; //�¿��п��С
			(FootLoc(t))->Uplink = *p;//�ײ�ָ��ָ���½���ͷ��

			if (*spa == t)
			{
				*spa = *p;
			}
		}
		else
		{
			s = (*p-1)->Uplink;//sָ��������(����ʱ)���׵�ַ
			t = *p + (*p)->size; //tָ��������(����ʱ)���׵�ַ
			
			(*p)->tag = 0;
			(FootLoc(*p))->tag = 0; 
			s->size += (*p)->size+t->size;//�����½��Ĵ�С
			t->Llink->Rlink = t->Rlink;//���t����
			t->Rlink->Llink = t->Llink;
			(FootLoc(t))->Uplink = s;	//�½��ײ�ָ��ָ����ͷ��

			if (*spa == t)
			{
				*spa = s;
			}
		}
	}
	*p = NULL;
}

//��ӡ��ǰ�����ڴ����ֹ��ַ
void print_free(PWord av,PWord *spa)
{
	assert(spa != NULL);
	PWord p = *spa;
	cout<<"���п���ֹ��Χ��";
	cout<<p-av+1<<"<--->"<<p+p->size-av<<" | ";
	while (p->Rlink != *spa)
	{
		p = p->Rlink;
		cout<<p-av+1<<"<--->"<<p+p->size-av<<" | ";
	}
	cout<<endl;
	

}

//��ӡ��ǰ��ռ�õ��ڴ�ĵ�ַ
void print_used(PWord mem,PWord pav)
{
	PWord r;
	r = pav;
	cout<<"�ǿ��п���ֹ��Χ��"<<endl;

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