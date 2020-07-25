# Simple-Shell


# Author 

Ben Keener
4/26/20
ben_keener@me.com

# Contents

# shell.c 
The main implementation of the shell script with an interactive mode and a mode that takes a script as an argument
# shell.h 
The header file for the shell that contains the struct command and the function free_command.
# parser.c 
The implementation of the functions contained in the command struct and free_command.
# Makefile
creates a shell  executable using shell.c and parser.c

# Running 

To run this program first execute the makefile then run the shell executable using ./shell. If you would like to use a scriptfile with commands the type ./shell scriptfile.txt into the command line with scriptfile.txt being your desired scriptfile. If there is no scriptfile then after the program executes, type your desired command then press enter. if you would like to exit the shell then type exit then press enter.

# Implementation notes

The do_builtin function executes all built in commands such as cd, setenv, etc. If the command is not in the preset built in commands then it goes to the do_external function. If anything fails and is not supposed to then an error is thrown using perror(). Each function will return -1 upon failure and 0 upon success. 

# Limitations

The buffer size for fgets is 1024, so it cannot take more that 1024 bytes as an argument.

# References 

I used the book Practical UNIX Programming by Kay A Robbins for ideas on how to get started on the shell and the structure as well as the idea for the Interactive and Non-Interactive modes. There were not any code snippets from the book that i used.

