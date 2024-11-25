#include "stdint.h"
#include <rvp_intrinsic.h>
#include "common.h"

extern int8_t in1[CONV_IN_SIZE][CONV_IN_SIZE];
extern int8_t wgts1[CONV_FEAT_SIZE][CONV_KERN_SIZE][CONV_KERN_SIZE];
extern volatile int8_t out1[CONV_OUT_SIZE][CONV_OUT_SIZE][CONV_FEAT_SIZE];

int main()
{
    puts("");
    for (int f = 0; f < CONV_FEAT_SIZE; ++f)
    {
        for (int i1 = 0; i1 < CONV_OUT_SIZE; ++i1)
        {
            for (int j1 = 0; j1 < CONV_OUT_SIZE; ++j1)
            {
                int32_t sum = 0;
                for (int i2 = 0; i2 < CONV_KERN_SIZE; ++i2)
                {
                    for (int j2 = 0; j2 < CONV_KERN_SIZE; j2 += 4)
                    {
                        const int32_t feat_wgt_vec = *(int32_t *)&wgts1[f][i2][j2];
                        const int32_t pix_pix = *(int32_t *)&in1[i1 + i2][j1 + j2];
                        sum = __rv_smaqa(sum, pix_pix, feat_wgt_vec);
                    }
                }
                out1[i1][j1][f] = sum >> 3;
            }
        }
    }
    puts("");
    return 0;
}
