#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

int main()
{
    srand(1);
    for (;;)
    {
        double interval = 10; /* seconds */

        /* start time */
        time_t start = time(NULL);

        /* do something */
        int duration = rand() % 13;
        printf("%2d seconds of work started at %s", duration, ctime(&start));
        sleep(duration);

        /* end time */
        time_t end = time(NULL);

        /* compute remaining time to sleep and sleep */
        double elapsed = difftime(end, start);
        int seconds_to_sleep = (int)(interval - elapsed);
        if (seconds_to_sleep > 0)
        { /* don't sleep if we're already late */
            sleep(seconds_to_sleep);
        }
    }
    return 0;
}