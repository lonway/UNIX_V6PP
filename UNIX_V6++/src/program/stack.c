#include <stdio.h>
#include <stdlib.h>
#include <sys.h>

/* APP1   ��ֹӦ�ó�������ں˿ռ䡣   �ں����ź�ɱ�������н���
int main1(int argc, char* argv[])
{


	int* pointer = (unsigned int*)0xC0300000;    //0x30000��ַ��Ҳ�ᵼ�½��̱�ɱ��

        *pointer = -1;

	printf("over\n");
	return 1;
}
*/

//APP2     ���������й����а�����չ��ջ��˳���������
void test(int n)
{
	int a[100];
	int i;
	if (n > 0) {
		for (i = 0; i < 100; i++)
			a[i] = i+1;
		printf("%d\n",n);
		test(n-1);
	}
	else {
		printf("0\n");
	}
}

int main1(int argc, char* argv[])
{
	printf("%d\n",sizeof(int));

	test(100);

	printf("over\n");
	return 1;
}



/*APP3     �������ͨ���� exec��ԭ����out of memory(�ľ��û��ռ�)
void test(int n)
{
	int a[500];
	int i;
	if (n > 0) {
		for (i = 0; i < 100; i++)
			a[i] = i+1;
		printf("%d\n",n);
		test(n-1);
	}
	else {
		printf("0\n");
	}
}

char dataArray[1019 *4096]; 

int main1(int argc, char* argv[])
{
	printf("%d\n",sizeof(int));

	test(300);

	printf("over\n");
	return 1;
}
*/

/*APP4        �������ͨ�ù� exec�����޷����н�����ԭ���Ƕ�ջ��չ�󣬽��̺ľ����û��ռ䣬ȱҳ�жϴ���������ź���ֹ���������
void test(int n)
{
	int a[500];
	int i;
	if (n > 0) {
		for (i = 0; i < 100; i++)
			a[i] = i+1;
		printf("%d\n",n);
		test(n-1);
	}
	else {
		printf("0\n");
	}
}

char dataArray[1015 *4096];

int main1(int argc, char* argv[])
{
	printf("%d\n",sizeof(int));

	test(300);

	printf("over\n");
	return 1;
}
*/

/* APP5   ��ֹӦ�ó���д����Ρ�   �ں����ź�ɱ�������н���
int main1(int argc, char* argv[])
{


	int* pointer = (unsigned int*)0x408000;

        *pointer = -1;

	printf("over\n");
	return 1;
}
*/

/* APP6 test for break
extern errno;
int main1(int argc, char* argv[])
{
	extern errno;
	printf(" begin \n");
	if( brk(1019*4096) < 0 ) // brk(2*1024*1024)���Գɹ���
	{
		printf(" break failed \n");
		if(errno == 12)
			printf(" because of Out Of Memory \n");
		//return 1;
		exit(0);
	}

	printf(" break over\n");

	exit(0);
	//return 1;
}
*/



