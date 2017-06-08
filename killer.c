#include <stdio.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/ipc.h>
#include <sys/shm.h>

void kill_producer();
void delete_memory();
void delete_processes_on_waiting();
void main(void)
{
    kill_producer();
}

void kill_producer()
{
    pid_t pid;
    key_t MyKey;
    int ShmID;
    pid_t *ShmPTR;
    char line[100], c;
    int i;

    MyKey = ftok(".", 's');
    ShmID = shmget(MyKey, sizeof(pid_t), 0666);
    ShmPTR = (pid_t *)shmat(ShmID, NULL, 0);
    pid = *ShmPTR;
    shmdt(ShmPTR);

    while (1)
    {
        printf("Presione k para matar el proceso > ");
        gets(line);
        for (i = 0; !(isalpha(line[i])); i++)
            ;
        c = line[i];
        if (c == 'k' || c == 'K')
        {
            printf("About to send a SIGQUIT signal\n");
            kill(pid, SIGQUIT);
            printf("Done.....\n");
            delete_memory();
            printf("\n");
            delete_processes_on_waiting();
            exit(0);
        }
        else
            printf("Wrong keypress (%c).  Try again\n", c);
    }
}

void delete_memory()
{
    const char *filepath = "/tmp/mmapped.bin";
    int ret;
    ret = remove(filepath);

    if (ret == 0)
    {
        printf("File deleted successfully");
    }
    else
    {
        printf("Error: unable to delete the file");
    }
}

void delete_processes_on_waiting()
{
    const char *filepath = "/tmp/mmapped_pro_on_waiting.bin";
    int ret;
    ret = remove(filepath);

    if (ret == 0)
    {
        printf("File deleted successfully");
    }
    else
    {
        printf("Error: unable to delete the file");
    }
}