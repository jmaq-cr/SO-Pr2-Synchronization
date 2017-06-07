#include "algorithms.h"

algorithm_t g_algorithm;

int select_algorithm()
{
    bool valid = true;
    int code;
    do
    {
        valid = true;
        puts("Algoritmos:");
        puts("[1] - Best-Fit");
        puts("[2] - First-Fit");
        puts("[3] - Worst-Fit");
        printf("Ingrese su eleccion (1-3): ");
        scanf("%d", &code);
        switch (code)
        {
        case 1:
            g_algorithm = BEST_FIT;
            break;
        case 2:
            g_algorithm = FIRST_FIT;
            break;
        case 3:
            g_algorithm = WORST_FIT;
            break;
        default:
            puts("Opcion incorrecta.");
            valid = false;
            break;
        }
    } while (valid == false);

    return g_algorithm;
}

int search_index(JSON_Array *mem_arr, int process_size, algorithm_t algorithm)
{

    int n = json_array_get_count(mem_arr);

    int index = -1;
    int shouldBreak = false;
    for (int i = 0; i < n; i++)
    {
        JSON_Object *root_object = json_array_get_object(mem_arr, i);
        int free_space_actual = json_object_get_number(root_object, "free_space");
        int free_space_old = 0;
        if (free_space_actual >= process_size)
        {
            switch (algorithm)
            {
            case BEST_FIT:
                if (index != -1)
                    free_space_old = json_object_get_number(json_array_get_object(mem_arr, index), "free_space");
                if (index == -1)
                    index = i;
                else if (free_space_old > free_space_actual)
                    index = i;
                break;
            case WORST_FIT:
                if (index != -1)
                    free_space_old = json_object_get_number(json_array_get_object(mem_arr, index), "free_space");
                if (index == -1)
                    index = i;
                else if (free_space_old < free_space_actual)
                    index = i;
                break;
            case FIRST_FIT:
                index = i;
                shouldBreak = true;
                break;
            default:
                printf("Algoritmo incorrecto\n");
                break;
            }
            if (shouldBreak)
                break;
        }
    }
    return index;
}