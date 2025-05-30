#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "defs.h"
#include "io.h"
#include "logic.h"

/*
* Made by csd4805 aka Emmanouil Papadakis
*/
int main(void)
{
    char *user;
    char line[MAX_LINE_SIZE];
    int rv, n;

    char *commands[MAX_COMMANDS];
    
    /*
     * Initialize user and working directory
     */
    user = getlogin();
    if (!user) {
        fprintf(stderr, "\n\t[E] Cannot get logged user information\n");
        return EXIT_FAILURE;
    }

    /*
     * Main loop
     */

    prompt(user);
    rv = read_line(line, MAX_LINE_SIZE);
    while ( rv != EOF )
    {
        n = parse_line(line, commands);
        run_commands(commands, n);

        prompt(user);
        rv = read_line(line, MAX_LINE_SIZE);
    }

    return 0;
}
