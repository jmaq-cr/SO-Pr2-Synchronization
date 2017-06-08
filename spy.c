#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>
#include "parson.h"

char *spy_memory();
void spy_on_memory();
void spy_on_waiting();

int main(int argc, const char *argv[])
{
    bool op_exit = false;
    int code;
    char *serialized_string = NULL;
    do
    {
        puts("Algoritmos:");
        puts("[1] - Estado de la memoria.");
        puts("[2] - Estado de procesos.");
        puts("[0] - Salir");
        printf("Ingrese su eleccion (0-2): ");
        scanf("%d", &code);
        switch (code)
        {
        case 0:
            op_exit = true;
            break;
        case 1:
            serialized_string = spy_memory();
            puts(serialized_string);
            break;
        case 2:
            printf("Procesos en espera:\n");
            spy_on_waiting();
            printf("Procesos en memoria:\n");
            spy_on_memory();
            break;
        default:
            puts("Opcion incorrecta.");
            break;
        }
    } while (op_exit == false);
    return 0;
}

char *spy_memory()
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

void spy_on_memory()
{
    char *serialized_string = NULL;
    serialized_string = spy_memory();
    //puts(serialized_string);
    JSON_Value *mem_value = json_parse_string(serialized_string);
    JSON_Array *mem_arr = json_value_get_array(mem_value);
    int n = json_array_get_count(mem_arr);
    JSON_Value *to_print_value;
    for (int i = 0; i < n; i++)
    {
        JSON_Object *root_object = json_array_get_object(mem_arr, i);
        JSON_Object *processes = json_object_get_object(root_object, "processes");

        to_print_value = json_object_get_wrapping_value(processes);
        if (!json_value_equals(to_print_value, json_value_init_object()))
        {
            serialized_string = NULL;
            serialized_string = json_serialize_to_string_pretty(to_print_value);
            puts(serialized_string);
        }
    }
}

void spy_on_waiting()
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

    puts(serialized_string);

    // Don't forget to free the mmapped memory
    if (munmap(map, fileInfo.st_size) == -1)
    {
        close(fd);
        perror("Error un-mmapping the file");
        exit(EXIT_FAILURE);
    }

    // Un-mmaping doesn't close the file, so we still need to do that.
    close(fd);
}