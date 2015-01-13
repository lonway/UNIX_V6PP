#ifndef JOBS_H
#define JOBS_H


#define MAXJOB 100
#define null 0
#define empty -1
#define MAXWD 100

//#include "Process.h"

/* A description of a pipeline's state. */
typedef enum { JNONE = -1, JRUNNING = 1, JSTOPPED = 2, JDEAD = 4, JMIXED = 8 } JOB_STATE;

/* Values for the FLAGS field in the JOB struct below. */
#define J_FOREGROUND 0x01 /* Non-zero if this is running in the foreground.  */
#define J_NOTIFIED   0x02 /* Non-zero if already notified about job state.   */
#define J_JOBCONTROL 0x04 /* Non-zero if this job started under job control. */
#define J_NOHUP      0x08 /* Don't send SIGHUP to job if shell gets SIGHUP. */
#define J_STATSAVED  0x10 /* A process in this job had had status saved via $! */
#define J_ASYNC	     0x20 /* Job was started asynchronously */

typedef struct Job{
	char wd[100];			/* The working directory at time of invocation. */
//	Process *pipe;		/* The pipeline of processes that make up this job. */
	int p_pid;			/* The process ID of the process group (necessary). */
	int p_gid;			/* The process group ID of the process group (necessary). */
	JOB_STATE state;	/* The state that this job is in. */
	int flags;			/* Flags word: J_NOTIFIED, J_FOREGROUND, or J_JOBCONTROL. */
}Job;

typedef struct JobListNode{
	Job job;
	int nextNode;
}JobListNode;

JobListNode jobList[MAXJOB];		//后台任务队列
extern int jobsCount;

#endif



