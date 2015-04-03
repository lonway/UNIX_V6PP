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
		( strcmp(cmd, "cd") || \
		  strcmp(cmd, "jobs")  )

extern void LuanchJob(Job* job);

void LuanchProcess(Process* p, int infile, int outfile){
	printf("LuanchProcess \n");	///

	//���Builtin����
	if(IsBuiltin(p->command_name)){
		if(strcmp(p->command_name, "cd") == 0){
			//TODO
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
	}

	printf("child \n");	///
	char pathName[100];
	if(infile > 0){	//������һ��process
		if(p->fin != 0){	//process����Ҳ�������ļ�����ô����ǰ��ܵ������
			int myInfile = open(p->fin, 0);
			close(stdin);
			dup(myInfile);
			close(myInfile);
		}
		else{
			close(stdin);
			dup(infile);
			close(infile);
		}
	}

	if(p->fout != 0){	//process������������ļ�
		int myOutfile = creat(p->fout, 0x1ff);
		close(stdout);
		dup(myOutfile);
		close(myOutfile);

		//���ܵ������붨�򵽱�process������ļ�
		close(myOutfile);
		dup(outfile);
		close(outfile);
	}
	else if(outfile > 0){	//ֱ�ӽ�stdout������ܵ�������
		close(stdout);
		dup(outfile);
		close(outfile);
	}

	//����binĿ¼
	pathName[0] = 0;
	strcat(pathName, "/bin/");
	strcat(pathName, p->command_name);
//		TODO strcat(pathName, p->args[0]); �����args��������ͼ
	printf("%s \n", pathName);
	printf("Begin to execute \n");	///
	if(-1 == execv(pathName, p->args)){
		printf("\'%s\' is not an exist command or may not in this folder!\n", p->command_name);
	}
	exit(0);

}

void LuanchJob(Job* job){
	int state;

	printf("LuanchJob \n");	///
	Process* p;
	int pid;
	int mypipe[2], infile = -1, outfile = -1;

	for(p=job->first_process; p!=0; p=p->next){
		printf("first_process \n");	///
		//��������ܵ��ߣ��򴴽�ǰ�����֮��Ĺܵ�
		if(p->next != 0){
			printf("0\n");	///
			if(pipe(mypipe) < 0){
				printf("pipe error\n");
				exit(1);
			}
			outfile = mypipe[0];
		}

		printf("1\n");	///
		pid = fork();
		printf("2\n");	///
		if(pid == 0){
			printf("enter LuanchProcess");	///
			LuanchProcess(p, infile, outfile);
		}
		else{
			printf("pid: %d\n", pid);	///
			while(wait(&state) != pid);
		}

		//�����ܵ�
		if(infile != stdin){
			close(infile);
		}
		if(outfile != stdout){
			close(outfile);
		}

		infile = mypipe[1];
	}

}