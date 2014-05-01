#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h> 
#include <sys/types.h>
#include <sys/wait.h>
#include "shell.h"
#include "ligne_commande.h"


void affiche_prompt(){
	char hostname[128], env[123], cwd[128];
	strcpy(env, getenv("USER"));
	getcwd(cwd, sizeof(cwd));
	gethostname(hostname, sizeof(hostname));
	printf("%s@%s:~%s$ ", env, hostname, cwd);
	fflush(stdout);
}

void execute_ligne_commande(){
	int nb,flag, status, pid;
	char *** cmd = ligne_commande(&flag, &nb);
	if(cmd == NULL)
		return;
	if(flag==-1)
		perror(cmd[0][0]);
		
	if(strcmp(cmd[0][0], "exit")==0)
		exit(1);
	if((pid=fork()) == 0)
	{
		execvp(cmd[0][0], cmd[0]);
		perror(cmd[0][0]);
		exit(-1);
	}
	else if(flag != 1)
	{
		pid = waitpid(pid, &status,0);
		if(pid == -1)
			perror("waitpid error");
	}
	libere(cmd);
}
int lance_commande( int in, int out, char *com, char ** argv)
{
	int pid;
	if ((pid=fork()) == 0)
	{
		if(in != 0)
			dup2(0,in);
		if(out != 1)
			dup2(1,out);
		execvp(com, argv);
		perror(com);
		exit(-1);
	}else{
		return pid;
		}
}

