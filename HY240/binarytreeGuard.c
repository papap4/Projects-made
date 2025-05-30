#include "Movie.h"
#include "binarytreeGuard.h"


movie_t *sentinel;

void initialize_movie_categories()
{
    sentinel = (movie_t *) malloc(sizeof(movie_t));
    if( !sentinel ){
        fprintf(stderr,"[E] Cannot create sentinel.\n");
        return;
    }

    sentinel->movieID  = -1;
    sentinel->lc = sentinel->rc = NULL;
    sentinel->year = sentinel->sumScore = sentinel->watchedCounter = 0;

    for(int i = 0; i < 6; i++) {
        categoryArray[i] = (movieCategory_t *)malloc(sizeof(movieCategory_t));
        categoryArray[i]->sentinel = sentinel;
        categoryArray[i]->movie = sentinel;
    }
}

int insert_movie(new_movie_t *new_movie){

    if ( !new_movie ) {
        fprintf(stderr, "[E] No new movie to insert.\n");
        return 0;
    }

    movie_t *movie = create_movie( new_movie->movieID, new_movie->year, new_movie->watchedCounter, new_movie->sumScore);
    int cat = new_movie->category;

    if( !movie ){
        fprintf(stderr, "[E] Cannot create movie to insert.\n");
        return 0;
    }

    BT_PRINT("Inserting movie %d in category %d\n", movie->movieID, cat);

    /* Empty tree */
    if(categoryArray[cat]->movie == categoryArray[cat]->sentinel){
        BT_PRINT("Inserting movie %d in category %d\n", movie->movieID, cat);
        categoryArray[cat]->movie = movie;
        return 1;
    }

    for(movie_t *p = categoryArray[cat]->movie; p!= NULL;){
        if(movie->movieID < p->movieID ){

            if(p->lc == sentinel){
                
                p->lc = movie;
                return 1;
            }
            p = p->lc;
            
        }
        else if(movie->movieID > p->movieID){
            if(p->rc == sentinel){
                
                p->rc = movie;
                return 1;
            }
            p = p->rc;
        }
    }
}

void print_my_movies(movie_t *root)
{
    if (!root || root == sentinel) {
        return;
    }

    print_my_movies(root->lc);
    printf("%d ", root->movieID);
    print_my_movies(root->rc);
}
movie_t *create_movie( int movieID, int year, int watchedCounter, int sumScore){

    movie_t *movie = (movie_t *)malloc(sizeof(movie_t));

    
    if(!movie){
        /* Debug prints should be inserted here*/
        return NULL;
    }

    movie->movieID = movieID;
    movie->year = year;
    movie->watchedCounter = watchedCounter;
    movie->sumScore = sumScore;

    movie->lc = sentinel;
    movie->rc = sentinel;
    BT_PRINT("Movie created from new movie\n");
    return movie;
}