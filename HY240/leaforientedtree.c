#include "Movie.h"
#include "leaforientedtree.h"

int insert_user_movie(userMovie_t **root, userMovie_t *user_movie)
{
    if ( !root ) {
        LT_PRINT("[E] No user's history to add in.\n");
        return 0;
    }

    if (!user_movie) {
        LT_PRINT("[W] No user movie to add in user's history.\n");
        return 0;
    }

    /* Empty tree */
    if ( !*root ) {
        *root = user_movie;
        return 1;
    }

    for (userMovie_t *p = *root; p; )
    {
        if ( user_movie->movieID < p->movieID ) {
            if ( p->lc ) {
                p = p->lc;
            } else {
                if (p->movieID > user_movie->movieID ) {
                    p->rc = create_user_movie(p->movieID, p->category, p->score, p);
                    p->category = p->score = -1;
                    p->lc = user_movie;
                    user_movie->parent = p;
                } else {
                    p->lc = create_user_movie(p->movieID, p->category, p->score, p);
                    p->category = p->score = -1;
                    p->movieID = user_movie->movieID;
                    p->rc = user_movie;
                    user_movie->parent = p;
                }
                return 1;
            }
        } else if ( user_movie->movieID > p->movieID ) {
            if ( p->rc ) {
                p = p->rc;
            } else {
                if (p->movieID > user_movie->movieID ) {
                    p->rc = create_user_movie(p->movieID, p->category, p->score, p);
                    p->category = p->score = -1;
                    p->lc = user_movie;
                    user_movie->parent = p;
                } else {
                    p->lc = create_user_movie(p->movieID, p->category, p->score, p);
                    p->category = p->score = -1;
                    p->movieID = user_movie->movieID;
                    p->rc = user_movie;
                    user_movie->parent = p;
                }
                return 1;
            }
        } else {
            LT_PRINT("[W] User movie #%d already in tree.\n", user_movie->movieID);
            return 0;
        }
    }
    return 0;
}

int remove_user_movie(userMovie_t **root, userMovie_t *child)
{
    userMovie_t *parent, *grandpa, *brother;

    if (root == NULL || *root == NULL || child == NULL) {
        return 0;
    }

    LT_PRINT("Removing: %d\n", child->movieID);

    parent = child->parent;
    grandpa = (parent != NULL) ? parent->parent : NULL;
    if ( parent != NULL ) {
        brother = (child == parent->lc) ? parent->rc : parent->lc;
    } else {
        brother = NULL;
    }

    int pv = ( parent ) ? parent->movieID : 0;
    int gpv = ( grandpa ) ? grandpa->movieID : 0;
    int bv = ( brother ) ? brother->movieID : 0;
    LT_PRINT("\tG: %d P: %d B: %d\n", gpv, pv, bv);

    LT_PRINT("Having both parent and grand parent?\n");

    /* Having full family path */
    if (grandpa != NULL) {
        
        if (parent == grandpa->lc) {
            grandpa->lc = brother;
        } else {
            grandpa->rc = brother;
        }
        free(parent); free(child);
        return 1;
    }

    LT_PRINT("Having parent only?\n");
    
    /* Having only parent, grand parent is missing */
    if ( parent != NULL ) {
        brother->parent = NULL;
        *root = brother;
        free(parent); free(child);
        return 1;
    }

    /* Removing root as the only node in the tree */
    
    LT_PRINT("Root is the only node.\n");
    free(*root);
    *root = NULL;

    return 0;
}

userMovie_t *find_user_movie(userMovie_t *root, int id)
{
    userMovie_t *p;

    if (!root) {
        return NULL;
    }

    for (p = root; p; )
    {
        if ( p->movieID == id && p->lc == NULL && p->rc == NULL) {
            return p;
        }

        p = (id < p->movieID) ? p->lc : p->rc;
    }

    return NULL;
}

void print_user_movies(userMovie_t *root)
{
    if (!root) {
        return;
    }

    print_user_movies(root->lc);
    int pv = ( root->parent ) ? root->parent->movieID : 0;
    printf("%d [C %d S %d | P %d] ", root->movieID, root->category, root->score, pv);
    print_user_movies(root->rc);
}


userMovie_t *create_user_movie(int id, Category_t category, int score, userMovie_t *parent)
{
    userMovie_t *user_movie = (userMovie_t *)malloc(sizeof(userMovie_t));

    if (!user_movie) {
        LT_PRINT("[E] Cannot create user movie #%d.\n", id);
        return NULL;
    }

    user_movie->movieID = id;
    user_movie->category = category;
    user_movie->score = score;

    user_movie->parent = parent;
    user_movie->lc = user_movie->rc = NULL;

    return user_movie;
}