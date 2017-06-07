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

void search_index(JSON_Array *mem_arr, int process_size)
{

    int i;
    int n = json_array_get_count(mem_arr);
    for (i = 0; i < n; i++)
    {
        JSON_Object *root_object = json_array_get_object(mem_arr, i);
        if (json_object_dothas_value(root_object, "processes.P200"))
            printf("Este lo tiene: %d\n", i);
    }

    // Find the best fit block for current process
    /*int index = -1;
    int shouldBreak = 0;
    for (int i = 0; i < n; i++)
    {
        if (blockSize[j] >= processSize[i])
        {
            switch (g_algorithm)
            {
            case BEST_FIT:
                if (index == -1)
                    index = j;
                else if (blockSize[index] > blockSize[j])
                    index = j;
                break;
            case WORST_FIT:
                if (index == -1)
                    index = j;
                else if (blockSize[index] < blockSize[j])
                    index = j;
                break;
            case FIRST_FIT:
                index = j;
                shouldBreak = 1;
                break;
            default:
                printf("Bad input, quitting!\n");
                break;
            }
            if (shouldBreak)
                break;
        }
    }*/
}