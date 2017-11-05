#include "mmalloc.h"

int main()
{
	Word mem[LEN];
	PWord spa;  //�����ڴ�����ʼ��ַ
	space_init(&spa,mem);//*spaָ������ڴ�ռ�������һ�����
	PWord av = spa;
	print_free(av,&spa);
	
	PWord p1,p2,p3,p4;
	p1 = (PWord)mmalloc(&spa,80);
	print_free(av,&spa);
	p2 = (PWord)mmalloc(&spa,32);
	print_free(av,&spa);
	mfree(&spa,&p1);
	print_free(av,&spa);
	p3 = (PWord)mmalloc(&spa,15);
	p4 = (PWord)mmalloc(&spa,64);
	print_free(av,&spa);
	print_used(mem,mem);
	mfree(&spa,&p3);
	print_free(av,&spa);
	print_used(mem,mem);
	mfree(&spa,&p2);
	print_free(av,&spa);
	print_used(mem,mem);

	mfree(&spa,&p4);
	print_free(av,&spa);
	print_used(mem,mem);
	return 0;
}