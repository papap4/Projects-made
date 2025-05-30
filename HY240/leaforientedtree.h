/* Uncomment the following line to enable debugging prints 
 * or comment to disable it */
// #define DEBUG

#ifdef DEBUG
#define LT_PRINT(...) fprintf(stderr, __VA_ARGS__);
#else  /* DEBUG */
#define LT_PRINT(...);
#endif /* DEBUG */

int insert_user_movie(userMovie_t **, userMovie_t *);
int remove_user_movie(userMovie_t **, userMovie_t *);
userMovie_t *find_user_movie(userMovie_t *, int);

void print_user_movies(userMovie_t *);
userMovie_t *create_user_movie(int, Category_t, int, userMovie_t *);
