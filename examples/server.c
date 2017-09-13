#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <string.h>
#include <semaphore.h>

struct shared
{
    sem_t mutex;
    char buffer[100];
} shared;

int main()
{
    int fd;
    struct shared *ptr;
    sem_t play;
    int i;

    fd = open("foo", O_RDWR | O_CREAT | O_TRUNC, (mode_t)0600);

    printf("Opened, fd is %d\n", fd);
    write(fd, &shared, sizeof(struct shared));

    ptr = (struct shared *)mmap(NULL, sizeof(struct shared), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    printf("Now sharing, ptr is %p\n", ptr);

    printf("Initiating semaphore\n");
    i = sem_init(&(ptr->mutex), 1, 0);
    strcpy(ptr->buffer, "This is a test");

    printf("Waiting on semaphore\n");
    sem_wait(&(ptr->mutex));
    printf("Done waiting on semaphore\n");
    printf("Pausing...\n");
    sleep(1);
    ptr->buffer[1] = 'a';
    sleep(1);
    ptr->buffer[2] = 'a';
    sleep(1);
    ptr->buffer[3] = 'a';
    sleep(1);
    ptr->buffer[4] = 'a';
    sleep(1);
    ptr->buffer[5] = 'a';
    sleep(1);
    ptr->buffer[6] = 'a';
    sleep(1);
    ptr->buffer[7] = 'a';
    printf("destroying semaphore\n");
    close(fd);

    i = sem_destroy(&(ptr->mutex));
    sleep(10);
}