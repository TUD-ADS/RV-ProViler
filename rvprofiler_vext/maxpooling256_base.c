#include "stdint.h"
#include "stdio.h"
#include "common.h"

extern int32_t in1[MAXPOOL_IN_SIZE][MAXPOOL_IN_SIZE][MAXPOOL_FEAT_SIZE];
extern volatile int32_t out1[MAXPOOL_OUT_SIZE][MAXPOOL_OUT_SIZE][MAXPOOL_FEAT_SIZE];

int main()
{
    puts("");
    for (int i1 = 0; i1 < MAXPOOL_OUT_SIZE; i1++)
    {
        for (int j1 = 0; j1 < MAXPOOL_OUT_SIZE; ++j1)
        {
            for (int f = 0; f < MAXPOOL_FEAT_SIZE; ++f)
            {
                out1[i1][j1][f] = INT32_MIN;
                for (int i2 = 0; i2 < 2; ++i2)
                {
                    for (int j2 = 0; j2 < 2; ++j2)
                    {
                        out1[i1][j1][f] = MAX(out1[i1][j1][f], in1[2 * i1 + i2][2 * j1 + j2][f]);
                    }
                }
            }
        }
    }
    puts("");

    return 0;
}
