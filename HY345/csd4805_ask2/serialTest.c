#include <stdio.h>
#include <pthread.h>

#define NUM_THREADS 10
pthread_mutex_t mutex;
pthread_cond_t cond;
int current_thread = 0;

void *thread_function(void *thread_id) {
    int id = (int)thread_id;

    while (1) {
        pthread_mutex_lock(&mutex);

        // Check if it's this thread's turn to execute
        if (id == current_thread) {
            // Perform the work
            printf("Thread %d is executing.\n", id);

            // Update the current_thread variable for the next thread
            current_thread = (current_thread + 1) % NUM_THREADS;

            // Signal the next thread to execute
            pthread_cond_signal(&cond);
        } else {
            // Wait until it's this thread's turn
            pthread_cond_wait(&cond, &mutex);
        }

        pthread_mutex_unlock(&mutex);
    }

    pthread_exit(NULL);
}

int main() {
    pthread_t threads[NUM_THREADS];

    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond, NULL);

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_create(&threads[i], NULL, thread_function, (void *)i);
    }

    // Wait for all threads to finish (you may need to add a termination condition in thread_function)
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);

    return 0;
}