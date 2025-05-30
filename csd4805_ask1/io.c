#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <limits.h>
#include "defs.h"
#include "io.h"

/**
 * @brief Redirects standard input to a file whose name is given as an argument
 * 
 * @param input The filename to redirect input from. If NULL, nothing happens.
 * @return int  The file descriptor associated with the file where input will be redirected from
 */
int redirect_input(char *input)
{
    int fd;

    /* if provided input filename is not NULL */
    if ( !input ) {
        return STDIN_FILENO;
    }

    /* open file with provided filename for read */
    fd = open(input, O_RDONLY);

    /* file failed to open */
    if (fd < 0) {
        fprintf(stderr, "[E] %d\n", errno);
        perror("Cannot open input redirection");
        return EOF;
    }

    return fd;
}

/**
 * @brief Redirects standard output to a file whose name is given as argument
 * 
 * @param output    The filename to redirect output to. If NULL nothing happens.
 * @param is_append If TRUE (1) output is appended to the file.
 *                  If FALSE (0) output overwrites the contents of the file.
 * @return int      The file descriptor associated with the file where output will be redirected
 */
int redirect_output(char *output, int is_append)
{
    int fd;

    /* if provided output filename is not NULL */
    if ( !output ) {
        return STDOUT_FILENO;
    }
    
    /* will output append to the file or not? */
    if (is_append) {
        fd = open(output, O_WRONLY | O_APPEND);
    } else {
        fd = open(output, O_WRONLY | O_TRUNC | O_CREAT, 0600);
    }
        
    /* file failed to open */
    if (fd < 0) {
        fprintf(stderr, "[E] %d\n", errno);
        perror("Cannot open output redirection");
        return EOF;
    }

    return fd;
}

/**
 * @brief   Read command line input, char by char until new line.
 *          Tokenize line putting special chars (>, <, >>, |) between spaces
 * 
 * @param line  Input buffer.
 * @param n     Size of buffer. If greater that MAX_LINE_SIZE it is truncated.
 * @return int  How many chars apart '\n' were read or EOF if ^D is pressed on empty line
 */
int read_line(char *line, int n)
{
    int i, ch;

    /* exit, if no buffer provided */
    if (!line) {
        fprintf(stderr, "\n\t[E] No buffer to read input in\n");
        return EOF;
    }

    /* truncate to max buffer size */
    if ( n > MAX_LINE_SIZE ) {
        n = MAX_LINE_SIZE;
    }

    /* read input */
    for (i = 0; i < n-1; i++)
    {
        ch = getchar();

        switch (ch) {
            /* ^D pressed with empty line*/
            case EOF:
                return EOF;
            
            /* line ended with return */
            case '\n':
                line[i] = 0;
                return i;
            
            /* is it a special char? */
            case '>':
            case '<':
            case '|':
                /* add space before, if there was not any */
                if ( i > 0 && ' ' != line[i-1] ) {
                    line[i++] = ' ';
                }
                
                /* add input char */
                line[i] = ch;

                /* it is append '>>' rather than create '>' */
                if ( '>' == ch && i > 1 && '>' == line[i-2]) {
                    /* overwrite first's '>' trailing space with a second '>' */
                    line[i-1] = ch;
                    line[i] = ' ';
                    
                    /* job done, get out of swich for next char */
                    break;
                }

                /* add space after */
                line[++i] = ' ';
                break;
            
            default:
                /* just buffer any other input char */
                line[i] = ch;
                break;
        }
    }

    /* don't forget to terminate string */
    line[i] = 0;

    /* return how many chars were read */
    return i;
}

/**
 * @brief   Prints user name and working directory,
 * 
 * @param user          Logged in user name
 */
void prompt(char *user)
{
    /* working directory has PATH_MAX size */
    char working_dir[PATH_MAX];

    /* get working directory */
    getcwd(working_dir, sizeof(working_dir));
    printf("csd4805-hy345sh@%s:%s$ ", user, working_dir);

    return;
}
