/* Uncomment the following line to enable debugging prints 
 * or comment to disable it */
#define DEBUG

#ifdef DEBUG
#define BT_PRINT(...) fprintf(stderr, __VA_ARGS__);
#else  /* DEBUG */
#define BT_PRINT(...);
#endif /* DEBUG */
void initialize_movie_categories();
int insert_movie(new_movie_t *);
void print_my_movies(movie_t *);
movie_t *create_movie(int, int, int, int);

