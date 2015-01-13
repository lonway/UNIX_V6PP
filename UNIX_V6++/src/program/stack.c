#include <stdio.h>
#include <stdlib.h>
#include <sys.h>

/* APP1   禁止应用程序访问内核空间。   内核用信号杀死现运行进程
int main1(int argc, char* argv[])
{


	int* pointer = (unsigned int*)0xC0300000;    //0x30000地址，也会导致进程被杀死

        *pointer = -1;

	printf("over\n");
	return 1;
}
*/

//APP2     进程在运行过程中按需扩展堆栈，顺利运行完毕
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



/*APP3     这个程序通不过 exec，原因是out of memory(耗尽用户空间)
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

/*APP4        这个程序通得过 exec，但无法运行结束。原因是堆栈扩展后，进程耗尽了用户空间，缺页中断处理程序用信号终止了这个进程
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

/* APP5   禁止应用程序写代码段。   内核用信号杀死现运行进程
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
	if( brk(1019*4096) < 0 ) // brk(2*1024*1024)可以成功。
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



