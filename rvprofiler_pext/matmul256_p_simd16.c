#include "stdint.h"
#include "stdio.h"
#include <rvp_intrinsic.h>
#include "common.h"

extern int16_t in1[MATMUL_SIZE][MATMUL_SIZE];
extern int16_t in2[MATMUL_SIZE][MATMUL_SIZE];
extern volatile int16_t out1[MATMUL_SIZE][MATMUL_SIZE];

int main()
{
    puts("");
    for (int i = 0; i < MATMUL_SIZE; i += 1)
    {
        for (int j = 0; j < MATMUL_SIZE; j += 1)
        {
            int32_t sum = 0;
            for (int k = 0; k < MATMUL_SIZE; k += 2)
            {
                sum = __rv_v_kmada(sum, *(int16x2_t *)&in1[i][k], *(int16x2_t *)&in2[j][k]);
            }
            out1[i][j] = sum;
        }
    }
    puts("");
    return 0;
}
