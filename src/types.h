/**
*  \class types.h
*  *  
*  *  \brief Library of the types used in the Shell project.
*  *  \author Muller Roberto
*  *  \author Paulo Margarido
*  *  \author  Pedro Paulo
*  *  \author Vinicius Baggio
*  *  \author Tiago Vieira
*  *  \version 1.0
*  *  \date    08/10/2007
*  *  \warning No warranty is given for you. The author have no responsability for shell use.
*  *   */


#ifndef _TYPES_H_
#define _TYPES_H_

#include "../libs/str_utils.h"
#include "../libs/path_utils.h"

/**
 * A enum that identify the command typed by the user
 */
typedef enum command_code {
	COMM_NOTFOUND = 0, //! Command not found
	COMM_DIRNOTFOUND, //! Directory not found
	COMM_EXIT, //! Exit command
	COMM_PWD, //! Pwd command
	COMM_HELP, //! Help command
	COMM_CD, //! Change current directory
	COMM_EXTERNAL, //! It's an external command
	COMM_NOP,	//!It's not a command
	COMM_JOBS, //!jobs command
    COMM_ECHO, //!echo command
    COMM_KILL, //!kill command
	COMM_FG, //!fg command
	COMM_BG	//!bg command
} command_code;


#define MAX 20
#define RUN "Running"
#define STP "Stopped"


// Process information
typedef struct{
	int  pid;
	char name[MAX];
	char status[MAX];
}process;

// Running or stopped processes list
typedef struct{
	int size;
	process list[MAX];
}processes;


#define BUFFER_SIZE 200
#define TOKENS 20
#define NUM_COMMANDS 32


#endif /* _TYPES_H_ */
