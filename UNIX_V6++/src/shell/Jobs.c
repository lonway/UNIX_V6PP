#include "CommandTree.h"
#include "Jobs.h"
#include "stdlib.h"
#include "malloc.h"

Job *firstJob;
Job *curJob;
int jobsCount;

//生成process
Process* GenerateProcess(struct commandNode* node){
	Process* process = (Process*)malloc(sizeof(Process));
	process->next = 0;
	process->fin = 0;
	process->fout = 0;
	if(node->commandName != 0){
		process->command_name = (char*)malloc(100);
		strcpy(process->command_name, node->commandName);
	}
	if(node->fin != 0){
		process->fin = (char*)malloc(100);
		strcpy(process->fin, node->fin);
	}
	if(node->fout != 0){
		process->fin = (char*)malloc(100);
		strcpy(process->fout, node->fout);
	}
	if(node->commandType == TPAR){
		process->kind =  1;
	}
	else{
		process->kind = 0;
	}
	return process;
}

//获取管道中的procee，并添加到job当中
void GetProcessInTFIL(struct commandNode* node, Process* lastProcess){
	Process* curProcess = GenerateProcess(&commandNodes[node->left]);
	lastProcess->next = curProcess;
	if(node->right > 0){
		if(&commandNodes[node->right].commandType != TFIL){
			curProcess->next = GenerateProcess(&commandNodes[node->right]);
		}
		else{
			GetProcessInTFIL(&commandNodes[node->right], curProcess);
		}
	}
}

//生成job
void GenerateJob( struct commandNode* node, int type){
	switch(type){
	case 1:{
		switch(node->commandType){
		case TFIL: GenerateJob(node, 2);break;
		case TPAR: GenerateJob(node, 3);break;
		case TCOM: GenerateJob(node, 4);break;
		defult: printf("error\n");
		}
	};break;
	case 2:{
		++jobsCount;
		Job* job = (Job*)malloc(sizeof(Job));
		job->next = 0;
		job->p_gid = jobsCount;
		job->first_process = GenerateProcess(&commandNodes[node->left]);	//	先取出管道的第一个process

		curJob->next = job;
		curJob = job;

		if(node->right > 0){
			if(commandNodes[node->right].commandType != TFIL){
				curJob->first_process->next = GenerateProcess(&commandNodes[node->right]);
			}
			else{
				GetProcessInTFIL(&commandNodes[node->right], curJob->first_process);
			}
		}
	};break;
	case 3:
	case 4:{
//		printf("commandName: %s\n fin: %s\n fout: %s\n///\n", node->commandName, node->fin, node->fout);	///
//		int i;	///
//		for(i=0; i<node->params; ++i){	///
//			printf("\n%s\n", node->args[i]);	///
//		}	///
		++jobsCount;
		Job* job = (Job*)malloc(sizeof(Job));
		job->next = 0;
		job->p_gid = jobsCount;
		job->first_process = GenerateProcess(node);
//		printf("job->first_process: %s %s\n", job->first_process->fin, job->first_process->fout);	///
		curJob->next = job;
		curJob = job;

//		printf("commandName: %s\n fin: %s\n fout: %s\n///\n", job->first_process->command_name,
//				job->first_process->fin, job->first_process->fout);	///
	};break;
	defualt:printf("error\n");break;
	}
}

//解析命令树
void ParseCommandTree( struct commandNode* node ){
	switch(node->commandType){
	case TLST: {
		if(node->left > 0){	//从左至右分析，左边肯定是一个job
//			printf("node->left: %s\n", commandNodes[node->left].commandName);	///
			GenerateJob(&commandNodes[node->left], 1);
			printf("rightNumber: %d\n", node->right);
		}
		if(node->right > 0){	//右边进行递归分析
//			printf("ParseCommandTree: %s\n", commandNodes[node->right].commandName);	///
			ParseCommandTree(&commandNodes[node->right]);
		}
	};break;
	case TFIL: {
		GenerateJob(node, 2);
	};break;
	case TPAR: { //先把圆括弧中的内容当成一 个process放到当前的job当中
//		printf("fin: %s\n fout: %s\n", node->fin, node->fout);	///
//		printf("commandName: %s\n", commandNodes[node->left].commandName);	///
		GenerateJob(node, 3);
	};break;
	case TCOM: {
//		printf("TCOM: %s\n", node->commandName);
		GenerateJob(node, 4);
	};break;
	}
}
