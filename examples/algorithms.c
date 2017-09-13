#include <stdio.h>
#include <stdlib.h>

typedef enum {
    BEST_FIT = 0,
    FIRST_FIT = 1,
    WORST_FIT = 2
} algorithm_t;

// Function to allocate memory to blocks as per worst fit
// algorithm
void worstFit(int blockSize[], int m, int processSize[],
              int n)
{
    // Stores block id of the block allocated to a
    // process
    int allocation[n];

    // Initially no block is assigned to any process
    memset(allocation, -1, sizeof(allocation));

    // pick each process and find suitable blocks
    // according to its size ad assign to it
    algorithm_t algorithm = BEST_FIT;
    for (int i = 0; i < n; i++)
    {
        // Find the best fit block for current process
        int wstIdx = -1;
        int shouldBreak = 0;
        for (int j = 0; j < m; j++)
        {
            if (blockSize[j] >= processSize[i])
            {
                switch (algorithm)
                {
                case BEST_FIT: 
                    if (wstIdx == -1)
                        wstIdx = j;
                    else if (blockSize[wstIdx] > blockSize[j])
                        wstIdx = j;
                    break;
                case WORST_FIT:
                    if (wstIdx == -1)
                        wstIdx = j;
                    else if (blockSize[wstIdx] < blockSize[j])
                        wstIdx = j;
                    break;
                case FIRST_FIT:
                    wstIdx = j;
                    shouldBreak = 1;
                    break;
                default:
                    printf("Bad input, quitting!\n");
                    break;
                }
                if (shouldBreak) break;
            }
        }

        // If we could find a block for current process
        if (wstIdx != -1)
        {
            // allocate block j to p[i] process
            allocation[i] = wstIdx;

            // Reduce available memory in this block.
            blockSize[wstIdx] -= processSize[i];
        }
    }

    printf("\nProcess No.\tProcess Size\tBlock no.\n");
    for (int i = 0; i < n; i++)
    {
        printf("   %d\t\t%d\t\t", (i + 1), processSize[i]);
        if (allocation[i] != -1)
            printf("%d", (allocation[i] + 1));
        else
            printf("Not Allocated");
        printf("\n");
    }
}

// Driver code
int main()
{
    int blockSize[] = {100, 500, 200, 300, 600};
    int processSize[] = {212, 417, 112, 426};
    int m = sizeof(blockSize) / sizeof(blockSize[0]);
    int n = sizeof(processSize) / sizeof(processSize[0]);

    worstFit(blockSize, m, processSize, n);

    return 0;
}