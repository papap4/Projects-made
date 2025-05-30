#include "Movie.h"
#include "binarytree.h"

int insert_new_movie(new_movie_t *new_movie)
{
    if (!new_movie) {
        BT_PRINT("[W] No movie to add in new releases.\n");
        return 0;
    }

    /* Empty tree */
    if (!new_releases) {
        new_releases = new_movie;
        return 1;
    }

    for (new_movie_t *p = new_releases; p; )
    {
        if ( new_movie->movieID < p->movieID ) {
            if ( p->lc ) {
                p = p->lc;
            } else {
                p->lc = new_movie;
                return 1;
            }
        } else if ( new_movie->movieID > p->movieID ) {
            if ( p->rc ) {
                p = p->rc;
            } else {
                p->rc = new_movie;
                return 1;
            }
        } else {
            BT_PRINT("[W] Movie #%d already in tree.\n", new_movie->movieID);
            return 0;
        }
    }
    return 0;
}

new_movie_t *remove_new_movie(int id)
{
    new_movie_t *tmp, *p;

    if (!new_releases) {
        BT_PRINT("[W] Cannot remove from emty tree.\n");
        return NULL;
    }

    BT_PRINT("[D] Is root to be removed?\n");
    if ( new_releases->movieID == id ) {
        return remove_new_movie_root(&new_releases);
    }

    BT_PRINT("[D] Find node to remove.\n");
    for ( p = new_releases; p; )
    {
        // int l = (p->lc ? p->lc->movieID : 0), r = (p->rc ? p->rc->movieID : 0), x = p->movieID;
        // printf("id=%d x=%d l=%d r=%d\n", id, x, l, r);

        if ( id < p->movieID ) {
            if ( p->lc && p->lc->movieID == id ) {
                tmp = remove_new_movie_root(&p->lc);
                return tmp;
            } else {
                p = p->lc;
            }
        } else {
            if ( p->rc && p->rc->movieID == id ) {
                tmp = remove_new_movie_root(&p->rc);
                return tmp;
            } else {
                p = p->rc;
            }
        }
    }

    BT_PRINT("[W] Movie #%d is not in the tree.\n", id);
    return NULL;
}

new_movie_t *remove_new_movie_root(new_movie_t **root)
{
    new_movie_t *tmp, *p, *q;

    if (!root || !*root) {
        BT_PRINT("[W] Cannot remove from emty tree.\n");
        return NULL;
    }

    tmp = *root;
    
    /* Is root the last node? */
    BT_PRINT("[D] Is root the last node?\n");
    if ( !( (*root)->lc || (*root)->rc ) ) {
        *root = NULL;
        return tmp;
    }

    /* Can we replace root with its left child? */
    BT_PRINT("[D] Can we replace with left child?\n");
    if ((*root)->lc && !(*root)->lc->rc) {
        q = (*root)->lc;
        q->rc = (*root)->rc;
        (*root) = q;
        return tmp;
    }

    /* Can we replace root with its right child? */
    BT_PRINT("[D] Can we replace with right child?\n");
    if ((*root)->rc && !(*root)->rc->lc) {
        q = (*root)->rc;
        q->lc = (*root)->lc;
        (*root) = q;
        return tmp;
    }

    /* Replace with left most right child */
    BT_PRINT("[D] Replace with left most right child.\n");
    for (p = (*root)->lc; p; p = p->rc)
    {
        if (p->rc && !p->rc->rc) {
            break;
        }
    }
    q = p->rc;
    p->rc = q->lc;

    q->rc = (*root)->rc;
    q->lc = (*root)->lc;
    (*root) = q;

    tmp->lc = tmp->rc = NULL;
    return tmp;
}

void print_new_movies(new_movie_t (*root))
{
    if (!root) {
        return;
    }

    print_new_movies(root->lc);
    printf("%d ", root->movieID);
    print_new_movies(root->rc);
}

new_movie_t *create_new_movie(int id, Category_t category, int year, int watched_counter, int sum_score)
{
    new_movie_t *new_movie = (new_movie_t *)malloc(sizeof(new_movie_t));

    if (!new_movie) {
        BT_PRINT("[E] Cannot create new movie #%d.\n", id);
        return NULL;
    }

    new_movie->movieID = id;
    new_movie->category = category;
    new_movie->year = year;
    new_movie->watchedCounter = watched_counter;
    new_movie->sumScore = sum_score;

    new_movie->lc = new_movie->rc = NULL;

    BT_PRINT("New movie created\n");
    return new_movie;
}
