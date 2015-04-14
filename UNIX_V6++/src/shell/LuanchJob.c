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
	if(p->fin != 0){	//process本身有输入文件，那么屏蔽前面管道的输出
		int myInfile = open(p->fin, 0111);
		if(myInfile < 0){	//从bin路径查找
			char infilePathName[100];
			infilePathName[0] = 0;
			strcat(infilePathName, "/bin/");
			strcat(infilePathName, p->fin);
			myInfile = open(infilePathName, 0111);
		}
//			printf("stdin: %d\n", stdin);	///
//			printf("myInfile: %d\n", myInfile); ///
		close(stdin);
		dup(myInfile);
		close(myInfile);
	}
	else if(infile > 0){	//存在上一级process
		printf("pipe to stdin\n");	///
		close(stdin);
		dup(infile);
		close(infile);
	}

//	printf("p->fout: %d\n", p->fout);	///
	if(p->fout != 0){	//process本身存在输出文件
//		printf("enter!\n");	///
		int myOutfile = creat(p->fout, 0x1ff);
//		printf("myOutfile: %d\n", myOutfile);	///
		close(stdout);
		dup(myOutfile);
		close(myOutfile);

		//将管道的输入（写端）定向到本process的输出文件
		if(outfile > 0){
			close(myOutfile);
			dup(outfile);
			close(outfile);
		}
	}
	else if(outfile > 0){	//直接将stdout输出到管道的输入
		printf("stdout to pipe\n"); ///
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
//	printf("pathName: %s\n", pathName);	///
//	printf("Begin to execute \n");	///
	if(-1 == execv(pathName, p->args)){
		printf("\'%s\' is not an exist command or may not in this folder!\n", p->command_name);
	}

	// TODO 如果execv返回无错则无法执行到这里
	printf("close\n"); 	///
	close(stdin);
	close(stdout);

	exit(0);
}

void LuanchJob(Job* job){
	int state;

//	printf("LuanchJob \n");	///
	Process* p = job->first_process;
	int pid;
	int mypipe[2], infile = -1, outfile = -1;

	for(p=job->first_process; p!=0; p=p->next){
		//如果包含管道线，则创建前后进程之间的管道
		if(p->next != 0){
			if(pipe(mypipe) < 0){
				printf("pipe error\n");
				exit(1);
			}
			outfile = mypipe[1];
		}

		pid = fork();
//		if(pid == 0){
////			printf("infile: %d \toutfile: %d\t", infile, outfile);	///
////			LuanchProcess(p, infile, outfile);
//			printf("child_pid: %d\n", getpid());
//			char str[] = "hello!";
//			close(mypipe[0]);
//			write(mypipe[1], str, strlen(str)+1);
//			close(mypipe[1]);
//			exit(0);
//		}
//		else{
////			printf("pid: %d\t", pid);	///
//			while(wait(&state) != pid);
//			///
//			printf("parent_pid: %d\n", getpid());
//			char str1[100];
//			close(mypipe[1]);
//			read(mypipe[0], str1, 100);
//			close(mypipe[0]);
//			printf("pipe: %s\n", str1);
//			///
////			printf("End\n");	///
//		}

		if(pid == 0){
//			printf("infile: %d \toutfile: %d\t", infile, outfile);	///
//			LuanchProcess(p, infile, outfile);
			///
			printf("child_pid: %d\n", getpid());
			close(mypipe[1]);
			char str1[100];
			read(mypipe[0], str1, 100);
			close(mypipe[0]);
			printf("pipe: %s\n", str1);
			///
			exit(0);
		}
		else{
			printf("pid: %d\t", pid);	///
			///
			printf("parent_pid: %d\n", getpid());
			close(mypipe[0]);
			char str[] = "hello!";
			write(mypipe[1], str, strlen(str)+1);
			while(wait(&state) != pid);
			close(mypipe[1]);
//			printf("pipe: %s\n", str);
			///
			printf("End\n");	///
		}

		//清理管道
//		printf("1\n");	///

		infile = mypipe[0];
		outfile = -1;

//		printf("4\n");	///
	}
}
