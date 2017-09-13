#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/mman.h>
#include <sys/syscall.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include <unistd.h>
#include <slog.h>
#include "random.h"
#include "algorithms.h"

void *worker_thread(void *arg);
char *read_memory();
void write_memory(char *serialized_string);
char *read_memory_processes_on_waiting();
void write_memory_processes_on_waiting(char *serialized_string);
void SIGINT_handler(int);
void SIGQUIT_handler(int);

algorithm_t g_algorithm;
sem_t sem_find_space;
int ShmID;
pid_t *ShmPTR;

int main()
{
    // Seed random number generator
    srand(time(NULL));
    int process_creation_delay = 0;
    sem_init(&sem_find_space, 0, 1);
    slog_init("log", "slog.cfg", 2, 3, 1);

    g_algorithm = select_algorithm();
    //For killer
    pid_t pid = getpid();
    key_t MyKey;

    if (signal(SIGINT, SIGINT_handler) == SIG_ERR)
    {
        printf("SIGINT install error\n");
        exit(1);
    }
    if (signal(SIGQUIT, SIGQUIT_handler) == SIG_ERR)
    {
        printf("SIGQUIT install error\n");
        exit(2);
    }

    MyKey = ftok(".", 's');
    ShmID = shmget(MyKey, sizeof(pid_t), IPC_CREAT | 0666);
    ShmPTR = (pid_t *)shmat(ShmID, NULL, 0);
    *ShmPTR = pid;
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
        process_creation_delay = random_number(10, 15);
        printf("In main: delay of %d\n", process_creation_delay);
        sleep(process_creation_delay);
    }

    sem_destroy(&sem_find_space);
    pthread_exit(NULL);
}

void *worker_thread(void *arg)
{
    char *data;
    char *data_waiting;
    //create thread
    printf("before calling pthread_create getpid: %d getpthread_self: %lu tid:%lu\n", getpid(), pthread_self(), syscall(SYS_gettid));
    char str_process_id[40];
    long int process_id = syscall(SYS_gettid);
    sprintf(str_process_id, "%lu", process_id);

    int process_size = random_number(1, 10);
    int process_delay = random_number(30, 40);
    printf("process_size: %d\n", process_size);
    printf("process_delay: %d\n", process_delay);
    printf("process_id: %lu\n", syscall(SYS_gettid));
    data_waiting = read_memory_processes_on_waiting();
    JSON_Value *waiting_value = json_parse_string(data_waiting);
    JSON_Object *waiting_object = json_value_get_object(waiting_value);
    json_object_set_null(waiting_object, str_process_id);
    //update processes on waiting
    char *serialized_string_waiting = NULL;
    serialized_string_waiting = json_serialize_to_string(waiting_value);
    write_memory_processes_on_waiting(serialized_string_waiting);
    //semaphore
    sem_wait(&sem_find_space);
    data_waiting = read_memory_processes_on_waiting();
    waiting_value = json_parse_string(data_waiting);
    waiting_object = json_value_get_object(waiting_value);
    json_object_remove(waiting_object, str_process_id);
    //update processes on waiting
    serialized_string_waiting = NULL;
    serialized_string_waiting = json_serialize_to_string(waiting_value);
    write_memory_processes_on_waiting(serialized_string_waiting);
    //read data
    data = read_memory();
    JSON_Value *mem_value = json_parse_string(data);
    JSON_Array *mem_arr = json_value_get_array(mem_value);
    int mem_index = search_index(mem_arr, process_size, g_algorithm);
    printf("mem_index: %d\n", mem_index);
    if (mem_index == -1)
    {
        slog(0, SLOG_NONE, "[%s] %lu No entró a memoria.", strclr(CLR_YELLOW, "WARN"), syscall(SYS_gettid));
        sem_post(&sem_find_space);
        pthread_exit(NULL);
    }
    else
    {
        JSON_Object *root_object = json_array_get_object(mem_arr, mem_index);
        int free_space = json_object_get_number(root_object, "free_space");
        int new_space = free_space - process_size;
        json_object_set_number(root_object, "free_space", new_space);
        JSON_Object *pro_obj = json_object_get_object(root_object, "processes");
        JSON_Value *sub_schema = json_value_init_object();
        JSON_Object *sub_schema_obj = json_value_get_object(sub_schema);
        json_object_set_number(sub_schema_obj, "size", process_size);
        json_object_set_number(sub_schema_obj, "delay", process_delay);
        str_process_id[40];
        process_id = syscall(SYS_gettid);
        sprintf(str_process_id, "%lu", process_id);
        json_object_set_value(pro_obj, str_process_id, sub_schema);
        //update mem
        char *serialized_string = NULL;
        serialized_string = json_serialize_to_string(mem_value);
        write_memory(serialized_string);
        slog(0, SLOG_NONE, "[%s] %lu Asignado Size:%d Delay:%d", strclr(CLR_GREEN, "ASIG"), syscall(SYS_gettid), process_size, process_delay);
        //free sem
        sem_post(&sem_find_space);
        //sleep
        sleep(process_delay);
        sem_wait(&sem_find_space);
        char *data_return;
        data_return = read_memory();
        JSON_Value *mem_value_return = json_parse_string(data_return);
        JSON_Array *mem_arr_return = json_value_get_array(mem_value_return);
        JSON_Object *root_object_return = json_array_get_object(mem_arr_return, mem_index);
        int free_space_return = json_object_get_number(root_object_return, "free_space");
        int new_space_return = free_space_return + process_size;
        json_object_set_number(root_object_return, "free_space", new_space_return);
        //remove process index
        char str[80];
        strcpy(str, "processes.");
        strcat(str, str_process_id);
        puts(str);
        json_object_dotremove(root_object_return, str);
        char *serialized_string_return = NULL;
        serialized_string_return = json_serialize_to_string(mem_value_return);
        write_memory(serialized_string_return);
        slog(0, SLOG_NONE, "[%s] %lu Devuelto Size:%d Index:%d", strclr(CLR_BLUE, "DEVO"), syscall(SYS_gettid), process_size, mem_index);
        sem_post(&sem_find_space);
    }

    pthread_exit(NULL);
}

char *read_memory()
{
    const char *filepath = "/tmp/mmapped.bin";

    int fd = open(filepath, O_RDONLY, (mode_t)0600);

    if (fd == -1)
    {
        perror("Error opening file for writing");
        exit(EXIT_FAILURE);
    }

    struct stat fileInfo = {0};

    if (fstat(fd, &fileInfo) == -1)
    {
        perror("Error getting the file size");
        exit(EXIT_FAILURE);
    }

    if (fileInfo.st_size == 0)
    {
        fprintf(stderr, "Error: File is empty, nothing to do\n");
        exit(EXIT_FAILURE);
    }

    printf("File size is %ji\n", (intmax_t)fileInfo.st_size);

    char *map = mmap(0, fileInfo.st_size, PROT_READ, MAP_SHARED, fd, 0);
    if (map == MAP_FAILED)
    {
        close(fd);
        perror("Error mmapping the file");
        exit(EXIT_FAILURE);
    }

    JSON_Value *output_value = NULL;
    output_value = json_parse_string(map);
    char *serialized_string = NULL;
    serialized_string = json_serialize_to_string_pretty(output_value);

    //puts(serialized_string);

    // Don't forget to free the mmapped memory
    if (munmap(map, fileInfo.st_size) == -1)
    {
        close(fd);
        perror("Error un-mmapping the file");
        exit(EXIT_FAILURE);
    }

    // Un-mmaping doesn't close the file, so we still need to do that.
    close(fd);

    return serialized_string;
}

void write_memory(char *serialized_string)
{

    /* Open a file for writing.
     *  - Creating the file if it doesn't exist.
     *  - Truncating it to 0 size if it already exists. (not really needed)
     *
     * Note: "O_WRONLY" mode is not sufficient when mmaping.
     */

    const char *filepath = "/tmp/mmapped.bin";

    int fd = open(filepath, O_RDWR | O_CREAT | O_TRUNC, (mode_t)0600);

    if (fd == -1)
    {
        perror("Error opening file for writing");
        exit(EXIT_FAILURE);
    }

    // Stretch the file size to the size of the (mmapped) array of char
    if (serialized_string == NULL)
    {
        perror("serialized_string");
        exit(1);
    }

    size_t textsize = strlen(serialized_string) + 1; // + \0 null character

    if (lseek(fd, textsize - 1, SEEK_SET) == -1)
    {
        close(fd);
        perror("Error calling lseek() to 'stretch' the file");
        exit(EXIT_FAILURE);
    }

    /* Something needs to be written at the end of the file to
     * have the file actually have the new size.
     * Just writing an empty string at the current file position will do.
     *
     * Note:
     *  - The current position in the file is at the end of the stretched 
     *    file due to the call to lseek().
     *  - An empty string is actually a single '\0' character, so a zero-byte
     *    will be written at the last byte of the file.
     */

    if (write(fd, "", 1) == -1)
    {
        close(fd);
        perror("Error writing last byte of the file");
        exit(EXIT_FAILURE);
    }

    // Now the file is ready to be mmapped.
    char *map = mmap(0, textsize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (map == MAP_FAILED)
    {
        close(fd);
        perror("Error mmapping the file");
        exit(EXIT_FAILURE);
    }

    strcpy(map, serialized_string);
    //puts(map);

    // Write it now to disk
    if (msync(map, textsize, MS_SYNC) == -1)
    {
        perror("Could not sync the file to disk");
    }

    // Don't forget to free the mmapped memory
    if (munmap(map, textsize) == -1)
    {
        close(fd);
        json_free_serialized_string(serialized_string);

        perror("Error un-mmapping the file");
        exit(EXIT_FAILURE);
    }

    // Un-mmaping doesn't close the file, so we still need to do that.
    close(fd);

    json_free_serialized_string(serialized_string);
}

char *read_memory_processes_on_waiting()
{
    const char *filepath = "/tmp/mmapped_pro_on_waiting.bin";

    int fd = open(filepath, O_RDONLY, (mode_t)0600);

    if (fd == -1)
    {
        perror("Error opening file for writing");
        exit(EXIT_FAILURE);
    }

    struct stat fileInfo = {0};

    if (fstat(fd, &fileInfo) == -1)
    {
        perror("Error getting the file size");
        exit(EXIT_FAILURE);
    }

    if (fileInfo.st_size == 0)
    {
        fprintf(stderr, "Error: File is empty, nothing to do\n");
        exit(EXIT_FAILURE);
    }

    printf("File size is %ji\n", (intmax_t)fileInfo.st_size);

    char *map = mmap(0, fileInfo.st_size, PROT_READ, MAP_SHARED, fd, 0);
    if (map == MAP_FAILED)
    {
        close(fd);
        perror("Error mmapping the file");
        exit(EXIT_FAILURE);
    }

    JSON_Value *output_value = NULL;
    output_value = json_parse_string(map);
    char *serialized_string = NULL;
    serialized_string = json_serialize_to_string_pretty(output_value);

    //puts(serialized_string);

    // Don't forget to free the mmapped memory
    if (munmap(map, fileInfo.st_size) == -1)
    {
        close(fd);
        perror("Error un-mmapping the file");
        exit(EXIT_FAILURE);
    }

    // Un-mmaping doesn't close the file, so we still need to do that.
    close(fd);

    return serialized_string;
}

void write_memory_processes_on_waiting(char *serialized_string)
{

    /* Open a file for writing.
     *  - Creating the file if it doesn't exist.
     *  - Truncating it to 0 size if it already exists. (not really needed)
     *
     * Note: "O_WRONLY" mode is not sufficient when mmaping.
     */

    const char *filepath = "/tmp/mmapped_pro_on_waiting.bin";

    int fd = open(filepath, O_RDWR | O_CREAT | O_TRUNC, (mode_t)0600);

    if (fd == -1)
    {
        perror("Error opening file for writing");
        exit(EXIT_FAILURE);
    }

    // Stretch the file size to the size of the (mmapped) array of char
    if (serialized_string == NULL)
    {
        perror("serialized_string");
        exit(1);
    }

    size_t textsize = strlen(serialized_string) + 1; // + \0 null character

    if (lseek(fd, textsize - 1, SEEK_SET) == -1)
    {
        close(fd);
        perror("Error calling lseek() to 'stretch' the file");
        exit(EXIT_FAILURE);
    }

    /* Something needs to be written at the end of the file to
     * have the file actually have the new size.
     * Just writing an empty string at the current file position will do.
     *
     * Note:
     *  - The current position in the file is at the end of the stretched 
     *    file due to the call to lseek().
     *  - An empty string is actually a single '\0' character, so a zero-byte
     *    will be written at the last byte of the file.
     */

    if (write(fd, "", 1) == -1)
    {
        close(fd);
        perror("Error writing last byte of the file");
        exit(EXIT_FAILURE);
    }

    // Now the file is ready to be mmapped.
    char *map = mmap(0, textsize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (map == MAP_FAILED)
    {
        close(fd);
        perror("Error mmapping the file");
        exit(EXIT_FAILURE);
    }

    strcpy(map, serialized_string);
    //puts(map);

    // Write it now to disk
    if (msync(map, textsize, MS_SYNC) == -1)
    {
        perror("Could not sync the file to disk");
    }

    // Don't forget to free the mmapped memory
    if (munmap(map, textsize) == -1)
    {
        close(fd);
        json_free_serialized_string(serialized_string);

        perror("Error un-mmapping the file");
        exit(EXIT_FAILURE);
    }

    // Un-mmaping doesn't close the file, so we still need to do that.
    close(fd);

    json_free_serialized_string(serialized_string);
}

//To kill process
void SIGINT_handler(int sig)
{
    signal(sig, SIG_IGN);
    printf("From SIGINT: just got a %d (SIGINT ^C) signal\n", sig);
    signal(sig, SIGINT_handler);
}

void SIGQUIT_handler(int sig)
{
    signal(sig, SIG_IGN);
    printf("From SIGQUIT: just got a %d (SIGQUIT ^\\) signal"
           " and is about to quit\n",
           sig);
    shmdt(ShmPTR);
    shmctl(ShmID, IPC_RMID, NULL);

    exit(3);
}
