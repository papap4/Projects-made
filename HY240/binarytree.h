/* Uncomment the following line to enable debugging prints 
 * or comment to disable it */
 #define DEBUG

#ifdef DEBUG
#define BT_PRINT(...) fprintf(stderr, __VA_ARGS__);
#else  /* DEBUG */
#define BT_PRINT(...);
#endif /* DEBUG */

int insert_new_movie(new_movie_t *);
new_movie_t *remove_new_movie(int);
new_movie_t *remove_new_movie_root(new_movie_t **);

void print_new_movies(new_movie_t *);
new_movie_t *create_new_movie(int, Category_t, int, int, int);
