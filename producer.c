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

int main()
{
    char *data;
    srand(time(NULL));
    algorithm_t algorithm = select_algorithm();

    data = read_memory();
    JSON_Value *mem_value = json_parse_string(data);
    JSON_Array *mem_arr = json_value_get_array(mem_value);

    int size = random_number(1, 10);
    int delay = random_number(20, 60);
    printf("process_size: %d\n", size);
    int mem_index = search_index(mem_arr, size, algorithm);
    printf("mem_index: %d\n", mem_index);

    JSON_Object *root_object = json_array_get_object(mem_arr, mem_index);
    int free_space = json_object_get_number(root_object, "free_space");
    int new_space = free_space - size;
    json_object_set_number(root_object, "free_space", new_space);
    JSON_Object *pro_obj = json_object_get_object(root_object, "processes");
    JSON_Value *sub_schema = json_value_init_object();
    JSON_Object *sub_schema_obj = json_value_get_object(sub_schema);
    json_object_set_number(sub_schema_obj, "size", size);
    json_object_set_number(sub_schema_obj, "delay", delay);
    char stringNum[40];
    int num = 1854;
    sprintf(stringNum, "%d", num);
    json_object_set_value(pro_obj, stringNum, sub_schema);
    //---------------
    JSON_Value *sub_schema2 = json_value_init_object();
    JSON_Object *sub_schema_obj2 = json_value_get_object(sub_schema2);
    json_object_set_number(sub_schema_obj2, "size", size);
    json_object_set_number(sub_schema_obj2, "delay", delay);
    char stringNum2[40];
    int num2 = 7000;
    sprintf(stringNum2, "%d", num2);
    json_object_set_value(pro_obj, stringNum2, sub_schema2);

    char *serialized_string = NULL;
    serialized_string = json_serialize_to_string_pretty(mem_value);

    puts(serialized_string);
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
