/**
*  \class Main Program
*  
*  \brief Main Program from Crash Shell. A Linux Shell made for the SO-II homework pourpouse.
*  \author Muller Roberto
*  \author Paulo Margarido
*  \author  Pedro Paulo
*  \author Vinicius Baggio
*  \author Tiago Vieira
*  \version 1.0
*  \date    08/10/2007
*  \bug In some kernel versions or some glibc versions the program crashes. The tests were done in Ubuntu Linux and the system is aprarently good..
*  \warning No warranty is given for you. The author have no responsability for shell use.
*   */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>
#include <pwd.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/wait.h>

#include "executioner.h"
#include "types.h"
//#include "jobs_handler.h"

/// Shell name
#define NAME "crash"

/// Exit simple function
#define EXIT_IF_I_MUST(x) if(x == COMM_EXIT || x == COMM_NOTFOUND || x == COMM_DIRNOTFOUND) return x; 

#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

// Need to be this way because of the handlers
static processes procs;	

// Starts the list
void init_list(processes *procs){
	procs->size = 0;
}

// List the processes (running or stopped) in the shell
void jobs(processes p){
	int i;
	
	if(p.size == 0)
		printf("There is no processes neither running nor stopped\n");
	else
		for(i = 0; i < p.size; i++)
			printf("[%d] [%d]\t%s\t%s\n", i + 1, p.list[i].pid, p.list[i].name, p.list[i].status);
}

//Encontra o processo na lista através do seu pid
int find_process(processes *pcs, int pid){
	int i = 0;

	while(i<MAX){
		if(pcs->list[i].pid == pid)return i;
		i++;
	}
	return -1;
}

// Insert
void insert_process(processes *procs, process proc){
	int i;

	i = procs->size;

	procs->list[i].pid = proc.pid;
	strcpy(procs->list[i].name, proc.name);
	strcpy(procs->list[i].status, proc.status);
	procs->size++;
}

// When a process ends running, removes it from the list
void remove_process(processes *pcs, int pid){
	int i, j, found;

	i = 0;
	found = 0;

	while(!found){
		fflush(stdout);

		if(pcs->list[i].pid == pid){

			for(j = i; j < pcs->size; j++){
				pcs->list[j] = pcs->list[j + 1];
			}
			pcs->size--;	
			found = 1;
		}
		i++;
	}
}

struct passwd *user_info;

/*
 * Funções que tratam os sinais quando o shell está em primeiro plano e os 
 * processos estão em segundo plano
 */


/*Variavel usada para diferenciar entre o SIGCONT e o SIGCHLD*/
int bgrunning = 0;
/*Variavel usada para diferenciar entre o SIGSTP e o SIGCHLD*/
int bgstopped = 0;



/*
 * Capturando o SIGINT em background
 */
void bgcatch_SIGINT(int signum)
{
}



/*
 * Capturando o SIGCONT em background
 */
void bgcatch_SIGCONT(int signum)
{
}


/*
 * Capturando o SIGSTP em background
 */
void bgcatch_SIGTSTP(int signum)
{
}

/*
 * Capturando o SIGCHLD em background
 */
void bgcatch_SIGCHLD(int signum)
{
	int estado;

	int filho = waitpid (-1, &estado, WUNTRACED | WCONTINUED);
	int i = find_process(&procs,filho);
	if (WIFSTOPPED(estado)){

		strcpy(procs.list[i].status,STP);
		printf("[%d] [%d]\t%s\t%s\n:>", i+1 , procs.list[i].pid, procs.list[i].name, procs.list[i].status);
	}
	else if (WIFCONTINUED(estado)){

		strcpy(procs.list[i].status,RUN);
		printf("[%d] [%d]\t%s\t%s\n", i+1 , procs.list[i].pid, procs.list[i].name, procs.list[i].status);
	}		
	else if(WIFEXITED(estado) || WIFSIGNALED(estado)){
		printf("[%d] [%d]\t%s\tDead\n:>", i+1 , procs.list[i].pid, procs.list[i].name);
		remove_process(&procs, filho);
	}
	fflush(stdout);
}

/**
 * Parses and executes a command
 * /param inner_command A command line, with an action and it's parameters
 * /return the command code
 */
command_code inner_parser(processes *procs, const char *inner_command, int background);


/**
 * Parses a command line entered by the user
 * /param command_line The full command line
 * /return returns the command code indicated by COMMAND_CODE
 */
command_code parse_command(processes *procs, char *command_line);

/**
 * Parses and executes a command
 * /param inner_command A command line, with an action and it's parameters
 * /return the command code
 */
command_code inner_parser(processes *procs, const char *inner_command, int background) {
	command_code comm_code = COMM_NOTFOUND;
	char splitted[TOKENS][BUFFER_SIZE] = {{0}};
	char *command = NULL;
	char *string_ptr = NULL;
	char *inner_context;
	int num_of_parameters = 0;
	int i;

	// Parses the command line
	string_ptr = strtok_r((char*)inner_command, " ", &inner_context);
	if(string_ptr) {
		strncpy(splitted[0], string_ptr, BUFFER_SIZE);
		string_ptr = strtok_r(NULL, " ", &inner_context);
		for(i = 1; i < TOKENS && string_ptr != NULL; i++) {
			strncpy(splitted[i], string_ptr, BUFFER_SIZE);
			string_ptr = strtok_r(NULL, " ", &inner_context);
			num_of_parameters++;
			
		}
		num_of_parameters++;
		command = splitted[0];
		
	} else {
		printf("Syntax error.\n");
		return COMM_NOTFOUND;
	}

	check_redirect(splitted, &num_of_parameters);

	if (!check_pipe(splitted, &num_of_parameters)) {
		comm_code = executioner(procs, command, splitted, num_of_parameters,background);
	}
	
	redirected();
	fflush(stdout);
	return comm_code;
}

/**
 * Parses a command line entered by the user
 * /param command_line The full command line
 * /return returns the command code indicated by COMMAND_CODE
 */
command_code parse_command(processes *procs, char *command_line) {
	command_code rets = COMM_NOTFOUND;
	char splitted[BUFFER_SIZE] = "";
	char *string_amps = NULL;
	char *parser_context;
	int i = 0;
	int background = 0;

	for(i = strlen(command_line); i > 0 && command_line[i] != '&'; i--) {
		if(command_line[i] != ' ' && command_line[i] != '\0') break;
	}

	if(command_line[i] == '&' && command_line[i-1] == ' ') {
		background = 1;
		command_line[i] = '\0';
	}
	i = 0;

	// First, try to get &&, so we'll run more than one command
	string_amps = strtok_r((char*)command_line, "&&", &parser_context);
	if(string_amps) {
		
		strncpy(splitted, string_amps, BUFFER_SIZE);

		string_amps = strtok_r(NULL, "&&", &parser_context);
							//if the last is in background and this is the last
		rets = inner_parser(procs, splitted,(background&&(!string_amps)));

		EXIT_IF_I_MUST(rets);

		splitted[0] = '\0';
		
		for(i = 1; i < TOKENS && string_amps != NULL; i++) {
			strncpy(splitted, string_amps, BUFFER_SIZE);
			string_amps = strtok_r(NULL, "&&", &parser_context);

							//if the last is in background and this is the last
			rets = inner_parser(procs, splitted,(background&&(!string_amps)));
			EXIT_IF_I_MUST(rets);
			
			splitted[0] = '\0';
		}

	}
	
	return rets;
}


/**
 * The main function
 */
int main(int argc, char **argv) {
	int quit = 0;
	char buffer[BUFFER_SIZE] = {};
	command_code comm_code;

	init_list(&procs);

	user_info = getpwnam(getlogin());
			
	printf("Welcome to %s!\n", NAME);

	while(!quit) {
		//Sinais capturados em Background
	    signal(SIGINT, bgcatch_SIGINT);
	    signal(SIGCONT, bgcatch_SIGCONT);
	    signal(SIGTSTP, bgcatch_SIGTSTP);
	    signal(SIGCHLD, bgcatch_SIGCHLD);

		fflush(stdout);
		do {
			printf(":> ");
		}while(!readline(buffer, BUFFER_SIZE));

//		printf("readline\n");
//		fflush(stdout);
		comm_code = parse_command(&procs, buffer);

		if(comm_code == COMM_EXIT) {
				quit = 1;
		}
		fflush(stdout);
	}
		
	printf("Buh-bye!\n");
	return 0;
}
