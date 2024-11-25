#include "stdint.h"
#include <rvp_intrinsic.h>
#include "common.h"

extern int16_t in1[FC_IN_SIZE];
extern int16_t wgts[FC_OUT_SIZE][FC_IN_SIZE];
extern volatile int16_t out1[FC_OUT_SIZE];

int main()
{
    puts("");
    for (int i1 = 0; i1 < FC_OUT_SIZE; ++i1)
    {
        int32_t feat = 0;
        for (int f = 0; f < FC_IN_SIZE; f += 2)
        {
            const int16x2_t wgt_wgt = *(int16x2_t *)&wgts[i1][f];
            const int16x2_t pix_pix = *(int16x2_t *)&in1[f];
            feat = __rv_v_kmada(feat, pix_pix, wgt_wgt);
        }

        out1[i1] = __rv_max(feat >> 3, 0);
    }
    puts("");
    return 0;
}
