#include "stdint.h"
#include "stdio.h"
#include <rvp_intrinsic.h>
#include "common.h"

extern int8_t in1[MAXPOOL_IN_SIZE][MAXPOOL_IN_SIZE][MAXPOOL_FEAT_SIZE];
extern volatile int8_t out1[MAXPOOL_OUT_SIZE][MAXPOOL_OUT_SIZE][MAXPOOL_FEAT_SIZE];

int main()
{
    puts("");
    for (int i1 = 0; i1 < MAXPOOL_OUT_SIZE; i1++)
    {
        for (int j1 = 0; j1 < MAXPOOL_OUT_SIZE; ++j1)
        {
            for (int f = 0; f < MAXPOOL_FEAT_SIZE; f += 4)
            {
                int8x4_t cur_max = {INT8_MIN, INT8_MIN, INT8_MIN, INT8_MIN};
                for (int i2 = 0; i2 < 2; ++i2)
                {
                    for (int j2 = 0; j2 < 2; ++j2)
                    {
                        const int8x4_t in = *(int8x4_t *)&in1[2 * i1 + i2][2 * j1 + j2][f];
                        cur_max = __rv_v_smax8(cur_max, in);
                    }
                }
                *(int8x4_t *)&out1[i1][j1][f] = cur_max;
            }
        }
    }
    puts("");
    return 0;
}