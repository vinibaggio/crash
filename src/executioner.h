/**
*  \class executioner.h
*  *  
*  *  \brief Library of the executioner.c
*  *  \author Muller Roberto
*  *  \author Paulo Margarido
*  *  \author  Pedro Paulo
*  *  \author Vinicius Baggio
*  *  \author Tiago Vieira
*  *  \version 1.0
*  *  \date    08/10/2007
*  *  \warning No warranty is given for you. The author have no responsability for shell use.
*  *   */


#ifndef _EXECUTIONER_H_
#define _EXECUTIONER_H_

#include "types.h"




/**
 * Print help for built-in commands
 */
void print_help();

/**
 * Print current path
 */
void print_curr_path();

/**
 * Verifies if I/O has been redirected and reverts to default state
 */
void redirected();

/**
 * Parameter treatment
 * /param splitted receives a vector of strings containing parameters
 * /param num_of_parameters contains the number of parameters in the given command
 */
void check_redirect(char splitted[TOKENS][BUFFER_SIZE], int *num_of_parameters);

/**
 * Execute a binary executable
 * /param program program to be executed 
 * /param arguments arguments (being 0 the name of the program itself)
 * /param num_of_params number of paramerters
 * /return the command code, -1 if error
 */
command_code exec_bin(processes *procs, const char *program, char arguments[TOKENS][BUFFER_SIZE], int num_of_params, int background);


/**
 * Execute a job depending on the command
 * /param command string containing the command
 * /param parameters other parameters
 * /param num_of_params number of parameters
 * /return something
 */
command_code executioner(processes *procs, const char *command, char parameters[TOKENS][BUFFER_SIZE], int num_of_params, int background );

/**
 * Ask with Daisy
 */
int check_pipe(char splitted[TOKENS][BUFFER_SIZE], int *num_of_parameters);


#endif /* _EXECUTIONER_H_ */
