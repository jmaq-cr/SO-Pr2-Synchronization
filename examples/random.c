#include <stdio.h>
#include <unistd.h>

int random_number(int min_num, int max_num);

int main(void)
{
    int i = 0;
    // Seed random number generator
    srand(time(NULL));
    do
    {
        printf("Min : 1 Max : 30 = %d\n", random_number(1, 30));
        printf("Min : 100 Max : 1000 = %d\n", random_number(100, 1000));
        usleep(100 * 1000);
        i++;
    } while (i != 30);

    return 0;
}

int random_number(int min_num, int max_num)
{
    int result = 0, low_num = 0, hi_num = 0;

    if (min_num < max_num)
    {
        low_num = min_num;
        hi_num = max_num + 1; // include max_num in output
    }
    else
    {
        low_num = max_num + 1; // include max_num in output
        hi_num = min_num;
    }

    result = (rand() % (hi_num - low_num)) + low_num;
    return result;
}