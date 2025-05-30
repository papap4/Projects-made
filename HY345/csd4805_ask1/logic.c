#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <limits.h>
#include <unistd.h>
// #include <sys/types.h>
#include <sys/wait.h>

#include "defs.h"
#include "io.h"
#include "logic.h"


/**
 * @brief   "cd -" changes to last working directory.
 *          Every time cd is executed, we copy current directory to last directory,
 *          just before current directory is changed with cd.
 */
char last_dir[PATH_MAX], current_dir[PATH_MAX];

/**
 * @brief   Execute cd command to change working directory.
 * 
 * @param cmd   cd command with its arguments as NULL terminated array of strings.
 */
void exec_cd(char *cmd[])
{
    /* the path constructed when "cd ~/directory" */
    char path_under_home[PATH_MAX];

    /* get current working directory to keep for "cd -" */
    getcwd(current_dir, sizeof(current_dir));

    /*
     * cd to home directory, that is either "cd" or "cd ~"
     *
     * If cmd[1] is NULL then there is no path argument, that is "cd".
     * If the first char of cmd[1] is '~', then it must be the only char for "cd ~"
     *
     * The case for "cd ~user" or "cd ~/directory", is covered later.
     */
    if ( NULL == cmd[1] || ('~' == *cmd[1] && '\0' == cmd[1][1]) ) {

        if (chdir(getenv("HOME")) ) {
            fprintf(stderr, "[E] %d\n", errno);
            perror("cd home failed");
            return;
        }

        /* copy previous working directory to last directory, only if cd was successful */
        strcpy(last_dir, current_dir);
        return;
    }

    /* cd to user's directory that is under home, that is "cd ~/directory" */
    if ( '~' == *cmd[1] && '/' == cmd[1][1] ) {
        /* concat home directory for ~, and the rest skipping ~ */
        sprintf(path_under_home, "%s%s", getenv("HOME"), cmd[1]+1);

        if (chdir(path_under_home) ) {
            fprintf(stderr, "[E] %d\n", errno);
            perror("cd home failed");
            return;
        }

        /* copy previous working directory to last directory, only if cd was successful */
        strcpy(last_dir, current_dir);
        return;
    }

    /* cd to another user's home directory, that is "cd ~user" */
    if ( '~' == *cmd[1] ) { 
        fprintf(stderr, "[W] cd to another user's home directory is not supported yet\n");
        return;
    }

    /* cd to previous working directory, that is cd - */
    if ( '-' == *cmd[1] ) {            
        
        /* if there is not last directory, then stay where you are */
        if ( !*last_dir ) {
            return;
        }
        
        if ( chdir(last_dir) ) {
            fprintf(stderr, "[E] %d\n", errno);
            perror("cd failed");
            return;
        }

        /* copy previous working directory to last directory, only if cd was successful */
        strcpy(last_dir, current_dir);
        return;
    }

    /* cd to some path */
    if ( chdir(cmd[1]) ) {
        fprintf(stderr, "[E] %d\n", errno);
        perror("cd failed");
        return;
    }
    strcpy(last_dir, current_dir);
    return;
}

/**
 * @brief   Run the command whose parts (tokens) are in an NULL terminated array of strings.
 * 
 * @param cmd       NULL terminated array of strings with command's parts.
 * @param fd_in     Input descriptor for redirection or pipe.
 * @param fd_out    Output descriptor for redirection or pipe.
 */
void spawn_process(char *cmd[], int fd_in, int fd_out)
{
    pid_t pid;

    /* end on either quit or exit */
    if ( !strcmp(cmd[0], CMD_QUIT) || !strcmp(cmd[0], CMD_EXIT)) {
        exit(EXIT_SUCCESS);
    }

    /* execute cd */
    if ( !strcmp(cmd[0], CMD_CD) ) {
        exec_cd(cmd);
        return;
    }
    
    pid = fork();
    
    /* fork failed for some reason */
    if ( pid < 0 ) {
        fprintf(stderr, "[E] %d\n", errno);
        perror("Fork failed");
        exit(EXIT_FAILURE);
    }
    
    /* this is child process */
    if ( 0 == pid ) {
        /* handle input redirection */
        if ( STDIN_FILENO != fd_in && EOF != fd_in ) {
            dup2(fd_in, STDIN_FILENO);
            close(fd_in);
        }
        
        /* handle output redirection */
        if ( STDOUT_FILENO != fd_out && EOF != fd_out ) {
            dup2(fd_out, STDOUT_FILENO);
            close(fd_out);
        }
        
        /* run the command with any arguments provided */
        execvp(cmd[0], cmd);   
    } else {
        /* This is parent (shell) process. Wait for child to complete. */
        waitpid(pid, NULL, 0);
    }

    return;
}

/**
 * @brief           Parse and run command.
 *                  Parse means redirect input and output, and create pipes if necessary.
 * 
 * @param command   Command, arguments, and special redirections or pipe symbols
 */
void run_command(char *command)
{
    unsigned int arg_cnt, is_append = FALSE;
    char *token;
    char *args[MAX_COMMANDS];
    int fd_pipe[2], fd_in = STDIN_FILENO, fd_out = STDOUT_FILENO;


    arg_cnt = 0;
    for (token = strtok(command, " "); token; token = strtok(NULL, " "))
    {
        // printf("%s\n", token);
        switch (*token) {
            case '<':
                fd_in = redirect_input(strtok(NULL, " "));
                break;

            case '>':
                is_append = '>' == *(token+1);
                fd_out = redirect_output(strtok(NULL, " "), is_append);
                break;

            case '|':
                /* terminate the command part, before the pipe */
                args[arg_cnt] = (char *)NULL;
                
                /* reset counter to start again for the part after the pipe */
                arg_cnt = 0;

                /* create the pipe*/
                pipe(fd_pipe);

                /* run the command before the pipe */
                spawn_process(args, fd_in, fd_pipe[STDOUT_FILENO]);
                
                close(fd_pipe[STDOUT_FILENO]);
                fd_in = fd_pipe[STDIN_FILENO];

                break;

            default:
                args[arg_cnt++] = token;
                break;
        }
    }

    /* run command, if there were pipes, this is the last part */
    args[arg_cnt] = (char *)NULL;
    spawn_process(args, fd_in, fd_out);

    return;
}

/**
 * @brief           Run each command issued in command line
 * 
 * @param commands  Commands issued in command line
 * @param n         How many commands there are
 */
void run_commands(char *commands[], int n)
{
    int i;

    for (i = 0; i < n; i++)
    {
        fprintf(stderr, "[%s]\n", commands[i]);
        /* end on either quit or exit */
        if ( !strcmp(commands[i], CMD_QUIT) || !strcmp(commands[i], CMD_EXIT) ) {
            exit(EXIT_SUCCESS);
        }

        run_command(commands[i]);
    }
    return;
}


/**
 * @brief           Parse command line (inspired from man(3) strtok)
 * 
 * @param line      command line to parse
 * @param tokens    Command line commands that are separated by ';'
 * @return int      How many commands, line has
 */
int parse_line(char line[], char *tokens[MAX_COMMANDS])
{
    int i, cnt;
    char *p;

    for (i = 0, p = line; i < MAX_COMMANDS; i++, p = NULL)
    {
        tokens[i] = strtok(p, ";");
        if ( !tokens[i] ) {
            break;
        }
    }

    return i;
}
