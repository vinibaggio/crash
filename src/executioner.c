/**
 * *  \class Executionerm
 * *  
 * *  \brief Manege pipes function and create the processes..
 * *  \author Muller Roberto
 * *  \author Paulo Margarido 
 * *  \author  Pedro Paulo
 * *  \author Vinicius Baggio
 * *  \author Tiago Vieira
 * *  \version 1.0
 * *  \date    08/10/2007
 * *  \warning No warranty is given for you. The author have no responsability for shell use.
 * *   */



#include "executioner.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <pwd.h>

extern struct passwd *user_info;
//extern processes procs;	// Need to be this way because of the handlers


// Global variables that are required for I/O redirecting
int redir_out = 0, redir_in = 0;
int fpout;
int fpin;
fpos_t pos;
fpos_t pos_in;

int fgstoped = 0;

/*
 * Funções que tratam os sinais quando o shell está em segundo plano e os 
 * processos estão em primeiro plano
 */

/*
 * Capturando o SIGINT em foreground
 */
void catch_SIGINT(int signum)
{
    fflush(stdout);
    signal(SIGINT, catch_SIGINT);
}

/*
 * Capturando o SIGCONT em foreground
 */
void catch_SIGCONT(int signum)
{
    fflush(stdout);
    signal(SIGCONT,catch_SIGCONT);
}

/*
 * Capturando o SIGSTP em foreground
 */
void catch_SIGTSTP(int signum)
{
	fgstoped = 1;
	signal(SIGTSTP,catch_SIGTSTP);
}

/*
 * Capturando o SIGCHLD em foreground
 */
void catch_SIGCHLD(int signum)
{
	signal(SIGCHLD,catch_SIGCHLD);
}


/**
 * Print help
 */
void print_help() {
	printf("\texit: Exits :(\n\tquit: Same as exit\n\tpwd: Print current working directory\n");
	printf("\tcd: Change current directory\n");
}
/**
 * Print current path
 */
void print_curr_path() {
	char buffer[BUFFER_SIZE] = {};
	getcwd(buffer, BUFFER_SIZE);
	printf("%s\n", buffer);
}


/**
 * Deals with pipes, executing the different comands and redirecting inputs and outputs
 * /param commands holds every command of the pipe to be executed
 * /param num_commands the total number of commands in the line
 */
void mario(char commands[NUM_COMMANDS][TOKENS][BUFFER_SIZE], int num_commands) {
	int status;
	int pipefds[NUM_COMMANDS][2];
	int i, j;
	int k;

	pid_t pids[NUM_COMMANDS];

	// Creates every pipe that is going to be needed during execution.
	for (i = 0; i < num_commands - 1; i++){
		pipe(pipefds[i]);
	}

	// Executes all the commands.
	signal(SIGINT, catch_SIGINT);
    signal(SIGTSTP, catch_SIGTSTP);
	signal(SIGCONT, catch_SIGCONT);
	signal(SIGCHLD, catch_SIGCHLD);

	for (i = 0; i < num_commands; i++) {
		pids[i] = fork();

		// If it's the child.
		if (pids[i] == 0) {
			// Close every unused pipe, leaving only the necessary ones. Important to keep processes from staying alive.
			for (j = 0; j < num_commands - 1; j++) {
				if (j != i - 1)
					close(pipefds[j][0]);
				if (j != i)
					close(pipefds[j][1]);
			}

			// Creates a temporary command to be used during the execvp function. Necessary because of the required
			// NULL values in the end of the string vector.
			int l = 0;
			char *v[TOKENS] = {{0}};
			k = 0;
			while (k < TOKENS) {
				if (strcmp(commands[i][k], " ") && commands[i][k][0] != '\0') {
					v[l++] = commands[i][k];
				}
				k++;
			}
			v[l] = '\0';

			// If it's the first process to be executed
			if (i == 0) {
				// Duplicates STDOUT into the pipe and executes the first command
				dup2(pipefds[i][1], 1);
				printf("%d\n", execvp(v[0], v));
				close(pipefds[i][1]);
			// If it's the last command
			} else if (i == num_commands - 1) {
				// Duplicates STDIN into the pipe and executes
				dup2(pipefds[i-1][0], 0);
				printf("%d\n", execvp(v[0], v));
				close(pipefds[i-1][0]);
			// If it's a command in between
			} else {
				// Duplicates both STDIN and STDOUT into the pipe and executes
				dup2(pipefds[i-1][0], 0);
				dup2(pipefds[i][1], 1);
				printf("%d\n", execvp(v[0], v));
				close(pipefds[i-1][0]);
				close(pipefds[i][1]);
			}
			// Child process exits
			exit(0);
		}
	}

	// Close all pipes in the parent process
	// No need to check the pid because every child process exits after executing its command
	for (j = 0; j < num_commands - 1; j++){
		close(pipefds[j][0]);
		close(pipefds[j][1]);
	}

	// Waits for every process to complete execution and then returns
	for (i = 0; i < num_commands; i++){
		waitpid(pids[i], &status,  WUNTRACED | WCONTINUED);
	}
}

/**
 * Separates the many commands in a line and then calls Mario to handle the pipes
 * /param splitted contains the whole command line
 * /param num_of_parameters has the number of parameters in the command line (that is, every token)
 */
void separate_commands (char splitted[TOKENS][BUFFER_SIZE], int num_of_parameters) {
	char comandos[NUM_COMMANDS][TOKENS][BUFFER_SIZE];
	int i = 0, j = 0, k = 0;

	while (i < num_of_parameters) {
		// If it's a pipe,
		if (splitted[i][0] == '|') {
			int l;
			// "Erases" the rest of the command.
			for (l = k; l < TOKENS; l++) comandos[j][l][0] = '\0';
			// Resets the number of tokens in a single command.
			k = 0;
			// Increases the number of commands.
			j++;
		}
		else {
			// Copies the token into the current command.
			strncpy(comandos[j][k++], splitted[i], BUFFER_SIZE);
		}
		i++;
	}

	// "Erases" the rest of the last command.
	while (k < TOKENS) {
		comandos[j][k++][0] = '\0';
	}

	// Calls Mario! =)
	mario(comandos, j+1);
}


/**
 * Checks for pipes in the command line
 * /param splitted holds the command line
 * /param num_of_parameters number of parameters in command lind
 */
int check_pipe(char splitted[TOKENS][BUFFER_SIZE], int *num_of_parameters) {
	int i;
	for (i = 1; i < *num_of_parameters; i++) {
		if (!strcmp(splitted[i], "|")) {
			// Calls the function that prepares piped commands to be executed (and then calls the function that executes the pipe =) )
			separate_commands(splitted, *num_of_parameters);
			return 1;
		}
	}
	return 0;
}

/**
 * Parameter treatment to check for input/output redirecting, also erasing these portions of commands
 * /param splitted receives a vector of strings containing parameters
 * /param num_of_parameters contains the number of parameters in the given command
 */
void check_redirect(char splitted[TOKENS][BUFFER_SIZE], int *num_of_parameters) {
	int i;
	int final_output = 0;
	int final_output1 = 0;
	int final_input = 0;
	char file_temp[BUFFER_SIZE];

	// For every parameter
	for (i = 1; i < *num_of_parameters; i++) {
		// If it's an output redirection, obtains the farthest position
		if (!strcmp(splitted[i], ">")) {
			final_output = i;
			strncpy(file_temp, splitted[i+1], BUFFER_SIZE);
			sprintf(splitted[i], " ");
			sprintf(splitted[i+1], " ");
		} else
		// If it's an append redirection, obtains its position
		if (!strcmp(splitted[i], ">>")) {
			final_output1 = i;
			strncpy(file_temp, splitted[i+1], BUFFER_SIZE);
			sprintf(splitted[i], " ");
			sprintf(splitted[i+1], " ");
		} else
		// If it's an input redirection, obtains its position
		if (!strcmp(splitted[i], "<")) {
			final_input = i;
			strncpy(file_temp, splitted[i+1], BUFFER_SIZE);
			sprintf(splitted[i], " ");
			sprintf(splitted[i+1], " ");
		}
	}

	// Actually closes STDIN and opens the given file
	if (final_input > 0) {
		redir_in = 1;
		fflush(stdin);
		fgetpos(stdin, &pos_in);
		fpin = dup(fileno(stdin));
		freopen(file_temp, "r", stdin);
	}

	// No else because both output and input may be redirected in the same line. It is bugged, however
	// Closes STDOUT and opens the file. When both > and >> are present, the farthest prevails
	if (final_output > 0 || final_output1 > 0) {
		redir_out = 1;
		fflush(stdout);
		fgetpos(stdout, &pos);
		fpout = dup(fileno(stdout));
		if (final_output > final_output1) {
			freopen(file_temp, "w", stdout);
		}	
		if (final_output < final_output1) {
			freopen(file_temp, "a", stdout);
		}
	}
}


/**
 * Verifies if I/O has been redirected and reverts STDIO to default state
 */
void redirected() {
	// Resets STDOUT
	if (redir_out) {
		fflush(stdout);
		dup2(fpout, fileno(stdout));
		close(fpout);
		clearerr(stdout);
		fsetpos(stdout, &pos);
	}

	// Resets STDIN
	if (redir_in) {
		fflush(stdin);
		dup2(fpin, fileno(stdin));
		close(fpin);
		clearerr(stdin);
		fsetpos(stdin, &pos_in);
	}

	redir_out = 0;
	redir_in = 0;
}


/**
 * Execute a binary executable
 * /param program program to be executed 
 * /param arguments arguments (being 0 the name of the program itself)
 * /param num_of_params number of paramerters
 * /return the command code, -1 if error
 */
command_code exec_bin(processes *procs, const char *program, char arguments[TOKENS][BUFFER_SIZE], int num_of_params, int background) {
	int no_path = 0;
	int i = 0;
	char path[BUFFER_SIZE] = {}; 
	char filename[BUFFER_SIZE] = {};
	DIR *dir = NULL;
	command_code comm_ret = COMM_NOP;

	getpath(program, path, BUFFER_SIZE);
	getfile(program, filename, BUFFER_SIZE);

	dir = opendir(path);
	
	// If the folder doesn't exist, returns command not found
	// NOTE: If there is no path, we should look in the $PATH with
	// execvp
	no_path = !strlen(path);
	if(!dir && !no_path) {
		comm_ret = COMM_NOTFOUND;
	} else {
		pid_t pid;
		int status; 
		
		// If it does, check for the file, or try $PATH
		if(no_path || fileexists(dir, filename)) {
			signal(SIGINT, catch_SIGINT);
    		signal(SIGTSTP, catch_SIGTSTP);
			signal(SIGCONT, catch_SIGCONT);
			signal(SIGCHLD, catch_SIGCHLD);
			if(!(pid = fork())) {  //Child process
				int exec_return;
				int j = 0;
				char *v[TOKENS] = {{0}}; // Dont care about the warnings, we are aware of them
				
				i = 0;
				while (i < num_of_params) {
					// Create a list of pointers to strings of arguments w/o
					// wasting memory
					if (strcmp(arguments[i], " ")) {
						v[j++] = arguments[i];
					}
					i++;
				}
				v[j] = '\0';
				
				exec_return = execvp(program, v);
				
				exit(exec_return);				
			} else {		//Crash
				// Getting the process information
				
				process proc;
				
				proc.pid = pid;

				strcpy(proc.name, program);
				strcpy(proc.status, RUN);

				insert_process(procs, proc);	// Inserting process in the list
				

				if(!background){
					waitpid (pid, &status, WUNTRACED | WCONTINUED);

					if(fgstoped){
						int number = find_process(procs,pid);
						if (number>=0)
							strcpy(procs->list[number].status,STP);
					}
					else {
						remove_process(procs,pid);		
					}
					fgstoped = 0;

				}

				redirected();					
				// Verify the exit status from the child process
				if(WIFEXITED(status)) {

					if(WEXITSTATUS(status) == 255) { 
						comm_ret = COMM_NOTFOUND;
					} else {
						comm_ret = COMM_EXTERNAL;		
					}
				}
			}
		} else {
			comm_ret = COMM_NOTFOUND;
		}
	}
	if(dir) closedir(dir);
	return comm_ret;
}

/**
 * Execute a job depending on the command
 * /param command string containing the command
 * /param parameters other parameters
 * /param num_of_params number of parameters
 * /return something
 */
command_code executioner(processes *procs, const char *command, char parameters[TOKENS][BUFFER_SIZE], int num_of_params, int background) {
	command_code comm_code = COMM_NOTFOUND;

	if (strlen(command) < 1) return COMM_EXTERNAL; // Technical adjustments for compatibility with mario (Gambiarra)

	if(!strcmp(command, "exit") || !strcmp(command, "quit")) {
		comm_code = COMM_EXIT;
	} else 
	if(!strcmp(command, "pwd")) {
		print_curr_path();
		comm_code = COMM_PWD;		
	} else
	if(!strcmp(command, "help")) {
		print_help();
		comm_code = COMM_HELP;
	} else
    if(!strcmp(command, "echo")) {
        char buff[BUFFER_SIZE * TOKENS] = {};
        int i = 0;
        
        if(num_of_params > 1) {
        
            snprintf(buff, BUFFER_SIZE, "%s", parameters[1]);
            if(num_of_params > 2) {
                for(i = 2; i < num_of_params; i++) {
                    //strncat(buff, parameters[i], BUFFER_SIZE);
                    snprintf(buff, BUFFER_SIZE, "%s %s", buff, parameters[i]);
                }
            }
        }
        printf("%s\n", buff);
        comm_code = COMM_ECHO;
    } else        
	if(!strcmp(command, "cd")) {
		if(num_of_params > 2) {
			printf("Invalid usage, consult the non-existant manual.\n");
			comm_code = COMM_CD;
		} else if(num_of_params <= 2) {
			if(!strcmp(parameters[1], "~") || num_of_params == 1) {
				chdir(user_info->pw_dir);
				print_curr_path();
				comm_code = COMM_CD;
			} else 
			if(direxists(parameters[1])) {
				chdir(parameters[1]);
				print_curr_path();
				comm_code =  COMM_CD;
			} else {
				comm_code = COMM_DIRNOTFOUND;
			}
		}
	} else 
	if (!strcmp(command,"jobs")) {
		jobs(*procs);
		comm_code = COMM_JOBS;		
	} else 
	if (!strcmp(command, "kill")) {
		int i, pid;
		sscanf(parameters[1],"%d", &pid);

        i = find_process(procs, pid);
        if(i == -1) {
            printf("This PID isn't mine or it doesn't exists at all.\n");
	    } else {
		    if(kill(pid, SIGTERM) == -1) {
                printf("Could not kill process %d.\n", pid);
    		}
	    }
	
		comm_code = COMM_KILL;  
	} else 
	if (!strcmp(command, "bg")){		
		int i;
		sscanf(parameters[1],"%d", &i);

		strcpy(procs->list[i-1].status,RUN);
//		printf("[%d] [%d]\t%s\t%s\n", i , procs->list[i-1].pid, procs->list[i-1].name, procs->list[i-1].status);

		kill(procs->list[i].pid, SIGCONT);
		// Implement a seeking code in order to verify if the process incidates exists
		comm_code = COMM_BG;
	} else 
	if(!strcmp(command, "fg")){
		int i,status;
		sscanf(parameters[1],"%d", &i);

		signal(SIGINT, catch_SIGINT);
	    signal(SIGTSTP, catch_SIGTSTP);
		signal(SIGCONT, catch_SIGCONT);
		signal(SIGCHLD, catch_SIGCHLD);

		kill(procs->list[i].pid, SIGCONT);
		strcpy(procs->list[i].status,RUN);
		
		waitpid (procs->list[i].pid, &status, WUNTRACED);

		comm_code = COMM_FG;		
		// Implement a seeking code in order to verify if the process incidates exists
	} else {
		// Try to run a binary, since its not built-in program
		comm_code = exec_bin(procs, command, parameters, num_of_params, background);
	}
	
	switch(comm_code) {
		case COMM_NOTFOUND:
		printf("Command not found, try again, or not.\n");
		break;
		
		case COMM_DIRNOTFOUND:
		printf("Directory not found, try 'ls'.\n");
		break;
			printf("executioner\n");

		default:
		break;
	}
	
	return comm_code;
}

