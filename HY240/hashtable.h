/* Uncomment the following line to enable debugging prints 
 * or comment to disable it */
// #define DEBUG

#ifdef DEBUG
#define HT_PRINT(...) fprintf(stderr, __VA_ARGS__);
#else  /* DEBUG */
#define HT_PRINT(...)
#endif /* DEBUG */

int initialize_hashtable();
void push(user_t **, user_t *);
user_t *pop(user_t **);
user_t *remove_user(user_t **, int);
user_t *find_user(user_t *, int);

void print_user_list(user_t *);
void print_user(user_t);

user_t *create_user(int);
