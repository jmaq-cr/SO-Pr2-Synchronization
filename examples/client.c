#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <semaphore.h>
#include <sys/mman.h>

struct shared
{
    sem_t mutex;
    char buffer[100];
} shared;

int main()
{
    int fd;
    struct shared *ptr;

    fd = open("foo", O_RDWR);
    printf("Opened, fd is %d\n", fd);
    //  write (fd, &shared, sizeof (struct shared));

    ptr = (struct shared *)mmap(NULL, sizeof(struct shared), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    printf("Now sharing, ptr is %p\n", ptr);

    sem_post(&(ptr->mutex));
    printf("This is in memory: %s\n", ptr->buffer);
    close(fd);
}