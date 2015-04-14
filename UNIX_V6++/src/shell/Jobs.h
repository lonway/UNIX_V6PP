#ifndef JOBS_H
#define JOBS_H

#define MAXJOB 100
#define NULL 0
//#define empty -1
#define MAXWD 100

typedef struct Process{
	struct Process* next;
	char* command_name;		//命令名
	char* fin;				//输入文件
	char* fout;				//输出文件
	char* args[10];			//命令参数
	int pid;
	int kind;				//进程的类型：0普通进程和1括弧内进程
	int status;
}Process;

typedef struct Job{
	struct Job* next;
	char wd[100];			/* The working directory at time of invocation. */
	Process* first_process;
	int p_gid;			/* The process group ID of the process group (necessary). */
}Job;

extern Job* firstJob;
extern Job* curJob;
extern int jobsCount;

extern void ParseCommandTree(struct commandNode* node);

#endif



