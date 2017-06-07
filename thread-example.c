#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include "random.h"

void *worker_thread(void *arg)
{
    printf("before calling pthread_create getpid: %d getpthread_self: %lu tid:%lu\n", getpid(), pthread_self(), syscall(SYS_gettid));
    sleep(10);
    pthread_exit(NULL);
}

int main()
{
    while (true)
    {
        pthread_t *my_thread = malloc(sizeof(*my_thread));
        int ret;

        printf("In main: creating thread\n");
        ret = pthread_create(my_thread, NULL, &worker_thread, NULL);
        if (ret != 0)
        {
            printf("Error: pthread_create() failed\n");
            exit(EXIT_FAILURE);
        }
        process_creation_delay = random_number(30, 60);
        sleep(process_creation_delay);
    }

    pthread_exit(NULL);
}