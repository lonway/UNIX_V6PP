#include "stdio.h"
#include "sys.h"
#include "string.h"
#include "globe.h"
#include "stdlib.h"
#include "file.h"

#include "Jobs.h"
#define stdin 0
#define stdout 1

#define IsBuiltin( cmd ) \
		( strcmp(cmd, "cd") & \
		  strcmp(cmd, "jobs")  )

extern void LuanchJob(Job* job);

void LuanchProcess(Process* p, int infile, int outfile){
//	printf("LuanchProcess \n");	///

	//检测Builtin命令
	if(IsBuiltin(p->command_name) == 0){
		if(strcmp(p->command_name, "cd") == 0){
			if(p->args[1] == 0){
				printf("No path!\n");
			}
			else{
				printf("p->args[1]: %d\n", p->args[1]);	///
				if(chdir((p->args[1])) == -1){
					printf("Invalid path!\n");
				}
			}
		}
		else if(strcmp(p->command_name, "jobs") == 0){
			printf("Jobs \n");	///
			Job* job = firstJob->next;
			while(job != 0){
				printf("[%d]", job->p_gid);
				Process* p = job->first_process;
				while(p != 0){
					printf("\t%s", p->command_name);
					p = p->next;
				}
				printf("\n");
				job = job->next;
			}
		}
		return;
	}

//	printf("child \n");	///
	if(infile > 0){	//存在上一级process或者是第一个process具有输入重定向
		if(p->fin != 0){	//process本身也有输入文件，那么屏蔽前面管道的输出
			int myInfile = open(p->fin, 0);
			if(myInfile < 0){
				char infilePathName[100];
				infilePathName[0] = 0;
				strcat(infilePathName, "/bin/");
				strcat(infilePathName, p->fin);
//				printf("fin: %s\n", infilePathName); ///
				myInfile = open(infilePathName, 0111);
			}
//			printf("stdin: %d\n", stdin);	///
//			printf("myInfile: %d\n", myInfile); ///
			close(0);
			dup(myInfile);
			close(myInfile);
//			printf("stdin: %d\n", stdin);	///
//			char str[100];	///
//			gets(str);	///
//			read(myInfile, str, 8);
//			printf("str: %s\n", str);	///
		}
		else{
			close(stdin);
			dup(infile);
			close(infile);
		}
	}

	if(p->fout != 0){	//process本身存在输出文件
		int myOutfile = creat(p->fout, 0x1ff);
		close(stdout);
		dup(myOutfile);
		close(myOutfile);

		//将管道的输入定向到本process的输出文件
		close(myOutfile);
		dup(outfile);
		close(outfile);
	}
	else if(outfile > 0){	//直接将stdout输出到管道的输入
		close(stdout);
		dup(outfile);
		close(outfile);
	}

	//搜索bin目录
	char pathName[100];
	pathName[0] = 0;
	strcat(pathName, "/bin/");
	strcat(pathName, p->command_name);
//	printf("p->command_name: %s\n", p->command_name);	///
	printf("pathName: %s\n", pathName);	///
	printf("Begin to execute \n");	///
	if(-1 == execv(pathName, p->args)){
		printf("\'%s\' is not an exist command or may not in this folder!\n", p->command_name);
	}
	exit(0);

}

void LuanchJob(Job* job){
	int state;

//	printf("LuanchJob \n");	///
	Process* p = job->first_process;
	int pid;
	int mypipe[2], infile = -1, outfile = -1;
//	printf("p->fin: %d", p->fin); ///
	if(p->fin != 0){
		infile = 1;	//第一个进程有输入重定向
	}

	for(p=job->first_process; p!=0; p=p->next){
//		printf("first_process \n");	///
		//如果包含管道线，则创建前后进程之间的管道
		if(p->next != 0){
			if(pipe(mypipe) < 0){
				printf("pipe error\n");
				exit(1);
			}
			outfile = mypipe[0];
		}

		pid = fork();
		if(pid == 0){
//			printf("infile: %d \noutfile: %d\n", infile, outfile);
//			printf("enter LuanchProcess\n");	///
			LuanchProcess(p, infile, outfile);
		}
		else{
			printf("pid: %d\n", pid);	///
			while(wait(&state) != pid);
			printf("End\n");
		}

		printf("1\n");	///

		//清理管道
		if(infile != stdin){
			close(infile);
		}

		printf("2\n");	///

		if(outfile != stdout){
			close(outfile);
		}

		printf("3\n");	///

		infile = mypipe[1];

		printf("4\n");	///
	}

}
