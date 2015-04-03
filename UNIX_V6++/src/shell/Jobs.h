#ifndef JOBS_H
#define JOBS_H

#define MAXJOB 100
#define NULL 0
//#define empty -1
#define MAXWD 100

typedef struct Process{
	struct Process* next;
	char* command_name;		//������
	char* fin;				//�����ļ�
	char* fout;				//����ļ�
	char* args[10];			//�������
	int pid;
	int kind;				//���̵����ͣ�0��ͨ���̺�1�����ڽ���
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



