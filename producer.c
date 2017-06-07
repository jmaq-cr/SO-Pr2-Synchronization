#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/mman.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include <unistd.h>
#include "parson.h"
#include "random.h"
#include "algorithms.h"

char *read_memory();

JSON_Value *create_thread(int p_thread_id);

int main()
{
    char *data;
    srand(time(NULL));
    data = read_memory();
    //printf("segment contains: \"%s\"\n", data);
    JSON_Value *mem_value = json_parse_string(data);
    JSON_Array *mem_arr = json_value_get_array(mem_value);

    /*int i;
    for (i = 0; i < json_array_get_count(mem_arr); i++)
    {
        JSON_Object *root_object = json_array_get_object(mem_arr, i);
        if (json_object_dothas_value(root_object, "processes.P200"))
            printf("Este lo tiene: %d\n", i);
    }*/

    /*JSON_Value *test = create_thread(-200000000);
    char *serialized_string = NULL;
    serialized_string = json_serialize_to_string_pretty(test);
    puts(serialized_string);*/

    //----
    select_algorithm();
    search_index(mem_arr, 5);
    //----timer
    // while (true)
    // {
    //     int interval = random_number(30, 60);
    //     printf("Interval (30s-60s): %d\n", interval);
    //     sleep(interval);
    // }
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

JSON_Value *create_thread(int p_thread_id)
{
    char stringNum[40];
    int num = p_thread_id;
    sprintf(stringNum, "%d", num);
    JSON_Value *schema = json_value_init_object();
    JSON_Object *schema_obj = json_value_get_object(schema);
    JSON_Value *sub_schema = json_value_init_object();
    JSON_Object *sub_schema_obj = json_value_get_object(sub_schema);
    int size = random_number(1, 10);
    int delay = random_number(20, 60);
    json_object_set_number(sub_schema_obj, "size", size);
    json_object_set_number(sub_schema_obj, "delay", delay);
    json_object_set_value(schema_obj, stringNum, sub_schema);
    return schema;
}
