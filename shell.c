#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/types.h>
#include "shell.h"

//different modes for the mian function. If there is a script file as an argument the mode = NON_INTERACTIVE if there is not then the mode = INTERACTIVE
#define NON_INTERACTIVE  0
#define INTERACTIVE  1

//this function determines the number of argumnets in the command line
int count_args(char **argv)
{
	int i = 0;
	while(argv[i] != NULL)
	{
		i++;
	}
	return i;
}

//This function was used in debugging but is not used in the actual implementation of the program
void dump_args( struct command *theCommand )
{
	int n_args=count_args(theCommand->args);

	for ( int i = 0; i < n_args; i++ )
	{
		printf("%s\n",theCommand->args[i]);
	}
}


//This function takes a command and if that command is one of the built in commands that I was required to hard-code then this function executed that command 
int do_builtin(struct command* builtin)
{
	char* ncommand = builtin->args[0];

	//compares the command to exit
	if(strcmp("exit", ncommand)==0)
	{
		//exits the shell and returns 0
		exit(0);
	}

	//compares the command to cd
	if(strcmp("cd", ncommand)==0)
	{
		char* newdir;

		int n_args = count_args(builtin->args);

		//if there is no argument then the command changes the directory to the home directory
		if(n_args == 1)
		{
			newdir = getenv("HOME");
			if(newdir == NULL)
			{
				//if the home directory is not set then the shell throws an error and returns -1 upon failure
				perror("Error: $HOME not set\n");
				return -1;
			}
			
		}
		else
		//makes the new directiry target the directory specified in the string directly after the cd command
		{
			newdir = builtin->args[1];
		}

		//changes the directory and if it fails then the shell throws an error and returns a -1 upon failure
		if (chdir(newdir) != 0)
		{
			perror("chdir failed");
			return -1;
		}

		return 0;
	}

	//compares the command to setenv
	if(strcmp("setenv", ncommand) == 0)
	{
		int n_args = count_args(builtin->args);
	
		//if there are the correct number of arguments then this sets the environtment variable
		if(n_args > 2)
		{

			//if setenv fails an error is thrown and -1 is returned upon failure
			if(setenv(builtin->args[1], builtin->args[2], 1) == -1)
			{
				perror("Setenv failed");
				return -1;
			}
			//if setenv does not fail then return 0 upon success
			else
			{
				return 0;
			}
		}
		//if called with only one argument, this unsets the environment variable and returns 0 upon success;
		else if (n_args == 2)
		{
			unsetenv(builtin->args[1]);
		       return 0;	
		}
		//if there are no arguments an error is thrown and -1 is returned
		else
		{
			perror("no arguments");
			return -1;
		}

	}

	//if the command does not exist, retun 1
	return 1;
}

void do_external(struct command* cmd)
{
	pid_t pid = fork();
	
	//if the child process cannot be created  then throw an error
	if(pid < 0)
	{
		perror("could not create child process");
		return;
	}

	//
	else if(pid == 0)
	{
		//this handles output redirection and redirects the stdout to out_redir
		if ( cmd->out_redir )
		{
			//if the file could not be opened exit and return 1, if dup2 fails exit and return 2
			int fd = open(cmd->out_redir, O_WRONLY | O_CREAT | O_TRUNC, 0666);
			if(fd < 0)
       		        {
       	                	perror("could not open file");
				exit(1);
                	}
	                if(dup2(fd, fileno(stdout)) < 0)
			{
                        	perror("dup2 failed");
	                        exit(2);
       	         	}
		}
		
		//this handles input redirectiona and redirects in_redir to stdin
		if ( cmd->in_redir )
                {
                        int fd = open(cmd->in_redir, O_RDONLY);
                        if(fd < 0)
                        {
                                perror("could not open file");
                                exit(1);
                        }
                        if(dup2(fd, fileno(stdin)) < 0)
                        {
                                perror("dup2 failed");
                                exit(2);
                        }
                }
		//if execvp fails then throw an error and return 3
                execvp(cmd->args[0],  cmd->args);
                perror("could not execute process");
                exit(3);
        }
        

	//if the process is exited by any other means this section catches a signal and if the child was killed by a signal the throw an error
	else
        {
                int status = 0;
                if (waitpid(pid, &status, 0) < 0)
                {
                        perror("could not wait for child");
                        return;
                }
		if (WIFEXITED(status))
		{
			if(WEXITSTATUS(status) != 0)
			{
				perror("Command exited");

			}
		}
		if (WIFSIGNALED(status))
		{
			if(WIFSIGNALED(status) != 0)
			{
				perror("command exited");
			}
		}
        }



}

int main(int argc, char **argv)
{
	FILE *input = stdin;
	int mode = INTERACTIVE;
	char buff[1024];
	char *prompt = "shell>";

	//if there is more than one argument then the second argument is read as a script and the mode is set to non interactive
	if(argc > 1)
	{
		
		if (( input = fopen(argv[1],"r")) == NULL )
		{
			perror("Could not open file");
			exit(1);
		}
		mode=NON_INTERACTIVE;
	}

	//if the mode is interactive then the argument number is 1 or 0 and the function takes one command at a time and runs do_builtin or do_external
	if ( mode == INTERACTIVE )
		printf("%s", prompt);
	while(fgets(buff, sizeof(buff), input)!=NULL)
	{

		struct command* new_command = parse_command(buff);
		if(count_args(new_command->args) > 0)
		{
			if(do_builtin(new_command) == 1)
			{
				do_external(new_command);
			}
		}	
		free_command(new_command);
		if ( mode == INTERACTIVE )
			printf("%s", prompt);
	}

return 0;
}

