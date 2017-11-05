#ifndef MMALLOC_H
#define MMALLOC_H

#include <assert.h>
#include <iostream>

using namespace std;

#define LEN 1000
#define E 2	//����ռ������������ռ���������ռ��E���֣�
#define FootLoc(p) p+(p)->size-1	//ָ��p��ָ���ĵײ�

typedef struct Word	//Word:�ڴ�������
{
	union
	{
		Word * Llink;	//ͷ����ָ��ǰ�����
		Word * Uplink;	//�ײ���ָ�򱾽��ͷ��
	};
	int tag;	//���־��0�����У�1��ռ�ã�ͷ����β������
	int size;	//ͷ���򣬿��С	
	Word *Rlink;	//ͷ����ָ���̽��
}Word,*PWord;

void space_init(PWord *spa,PWord mem);

//�����ڴ棨�״���Ϸ���
void * mmalloc(PWord *spa,int len);

//�����ڴ�
void mfree(PWord *spa,PWord *p);

//��ӡ��ǰ�����ڴ����ֹ��ַ
void print_free(PWord av,PWord *spa);

//��ӡ��ǰ��ռ�õ��ڴ�ĵ�ַ
void print_used(PWord mem,PWord pav);

#endif