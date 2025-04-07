#include "stdint.h"
#include "stdio.h"
#include <rvp_intrinsic.h>
#include "common.h"

extern int8_t in1[MATMUL_SIZE][MATMUL_SIZE];
extern int8_t in2[MATMUL_SIZE][MATMUL_SIZE];
extern volatile int8_t out1[MATMUL_SIZE][MATMUL_SIZE];

int main()
{
    puts("");
    for (int i = 0; i < MATMUL_SIZE; i += 1)
    {
        for (int j = 0; j < MATMUL_SIZE; j += 1)
        {
            int32_t sum = 0;
            for (int k = 0; k < MATMUL_SIZE; k += 4)
            {
                sum = __rv_v_smaqa(sum, *(int8x4_t *)&in1[i][k], *(int8x4_t *)&in2[j][k]);
            }
            out1[i][j] = sum;
        }
    }
    puts("");
    return 0;
}
