#include "stdio.h"
#include "stdlib.h"
#include "sys.h"

#include "CommandTree.h"
#include "ExecuteCommand.h"
#include "globe.h"
#include "PreExecute.h"
#include "string.h"
#include "Jobs.h"
#include "malloc.h"

char line[1000];
char* args[50];
int argsCnt;
int main1()
{
	char lineInput[512];
	getPath( curPath );	

	/* add_begin 创建任务链表 */
	firstJob = (Job*)malloc(sizeof(Job));
	firstJob->next = 0;
	curJob = firstJob;
	/* add_end */

	int root;
	while( 1 )
	{
		root = -1;
		argsCnt = 0;
		InitCommandTree();
		printf("[%s]#", curPath);
		gets( lineInput );		
		if ( strcmp( "shutdown", lineInput ) == 0 )
		{
			syncFileSystem();
			printf("You can safely turn down the computer now!\n");
			break;
		}
		argsCnt = SpiltCommand(lineInput);
		root = AnalizeCommand(0, argsCnt - 1, 0);

		/* add_begin */
		if(root >= 0){
			ParseCommandTree(&commandNodes[root]);

			Job* job = firstJob->next;

			int i = 0;
			while(job != 0){
				printf("[%d]\t", job->p_gid);
				Process* process = job->first_process;
				while(process != 0){
					printf("%s\t", process->command_name);
					process = process->next;
				}
				printf("\n");
				job = job->next;
			}

//			printf("Luanch\n");	///

			LuanchJob(firstJob->next);
		}
		/* add_end */

		/*	//add
		if ( root >= 0 )
		{
			ExecuteCommand( &commandNodes[root], 0, 0 );
		}
		*/	//add
	}

	return 0;
}
