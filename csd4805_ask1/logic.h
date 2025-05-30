/*
 * Built in commands
 */
#define CMD_QUIT    "quit"
#define CMD_EXIT    "exit"
#define CMD_CD      "cd"


/*
 * Functions
 */
void exec_cd(char * []);
void spawn_process(char * [], int, int);
void run_command(char *);
void run_commands(char * [], int);

int parse_line(char [], char * []);
