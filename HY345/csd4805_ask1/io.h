/*
 * Functions
 */
void prompt(char *);
int read_line(char *, int);

/*
 * Handle redirections and pipes
 */
int redirect_input(char *);
int redirect_output(char *, int);
