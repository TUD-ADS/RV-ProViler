#include "stdint.h"
#include "stdio.h"
#include "common.h"

extern int32_t in1[FC_IN_SIZE];
extern int32_t wgts[FC_OUT_SIZE][FC_IN_SIZE];
extern volatile int32_t out1[FC_OUT_SIZE];

int main()
{
    puts("");
    for (int i1 = 0; i1 < FC_OUT_SIZE; ++i1)
    {
        int32_t sum = 0;
        for (int f = 0; f < FC_IN_SIZE; ++f)
        {
            sum += in1[f] * wgts[i1][f];
        }
        out1[i1] = sum;
    }
    puts("");

    return 0;
}
