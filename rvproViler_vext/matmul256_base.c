#include "stdint.h"
#include "stdio.h"
#include "common.h"

extern int32_t in1[MATMUL_SIZE][MATMUL_SIZE];
extern int32_t in2[MATMUL_SIZE][MATMUL_SIZE];
extern volatile int32_t out1[MATMUL_SIZE][MATMUL_SIZE];

int main()
{
    puts("");
    for (int i = 0; i < MATMUL_SIZE; i += 1)
    {
        for (int j = 0; j < MATMUL_SIZE; j += 1)
        {
            out1[i][j] = 0;
            for (int k = 0; k < MATMUL_SIZE; k += 1)
            {
                out1[i][j] += in1[i][k] * in2[j][k];
            }
        }
    }
    puts("");

    return 0;
}
