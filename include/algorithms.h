// include guard
#ifndef ALGORITHMS_H
#define ALGORITHMS_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "parson.h"

typedef enum {
    BEST_FIT = 1,
    FIRST_FIT = 2,
    WORST_FIT = 3
} algorithm_t;

int select_algorithm();
int search_index(JSON_Array *mem_arr, int process_size, algorithm_t algorithm);
// end include guard
#endif