#include "Movie.h"
#include "hashtable.h"
#define hash_key(x) ((x)%hashtable_size)

/**
 * @brief       Initialize the users' hashtable
 * 
 * @return int  1 on success
 *              0 on failure
 */
int initialize_hashtable()
{
    hashtable_size = 1021;
    user_hashtable_p = (user_t **)malloc( hashtable_size * sizeof(user_t *));

    if (!user_hashtable_p) {
        HT_PRINT("[E] Cannot create hashtable\n");
        return 0;
    }

    HT_PRINT("[D] Hashtable created. Size is %d\n", hashtable_size);

    /* Initialize hashtable entries */
    for (int i = 0; i < hashtable_size; i++)
    {
        user_hashtable_p[i] = NULL;
    }

    HT_PRINT("[D] Hashtable entries nullified.\n");

    return 1;
}

void push(user_t **head, user_t *user)
{
    user->next = *head;
    *head = user;
    return;
}

user_t *pop(user_t **head)
{
    user_t *tmp;

    if (!head || !*head) {
        return NULL;
    }

    tmp = *head;
    *head = tmp->next;
    return tmp;
}

user_t *remove_user(user_t **head, int id)
{
    user_t *p, *tmp;

    if (!head || !*head) {
        return NULL;
    }

    if ( (*head)->userID == id ) {
        return pop(head);
    }

    for (p = *head; p; p = p->next)
    {
        if (p->next == NULL) {
            return NULL;
        }

        if (p->next->userID == id) {
            break;
        }
    }

    tmp = p->next;
    p->next = tmp->next;

    return tmp;
}

user_t *find_user(user_t *list, int id)
{
    user_t *p;

    if (!list) {
        return NULL;
    }

    for (p = list; p; p = p->next)
    {
        if (p->userID == id) {
            return p;
        }
    }

    return NULL;
}

void print_user_list(user_t *list)
{
    if (!list) {
        printf("Empty list\n\n");
        return;
    }

    for (user_t *p = list; p; p = p->next)
    {
        printf("#%d ", p->userID);
    }
    printf("\n\n");
    return;
}

void print_user(user_t user)
{
    printf("User %d\n", user.userID);
    return;
}

user_t *create_user(int id)
{
    user_t *user = (user_t *)malloc(sizeof(user_t));
    if (!user) {
        fprintf(stderr, "[E] Cannot create user entry for id #%d\n", id);
        return NULL;
    }
    HT_PRINT("User #%d created.\n", id);

    user->history = NULL;
    user->next = NULL;
    user->userID = id;

    return user;
}