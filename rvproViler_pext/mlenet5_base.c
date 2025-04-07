#include "stdint.h"

#define MAX(x, y) (((x) > (y)) ? (x) : (y))

#define CONV1_IN_SIZE 28
#define CONV1_KERN_SIZE 5
#define CONV1_FEAT_SIZE 6
#define CONV1_OUT_SIZE (CONV1_IN_SIZE - CONV1_KERN_SIZE + 1) // 24

extern int32_t in1_conv1[CONV1_IN_SIZE][CONV1_IN_SIZE];
extern int32_t weights_conv1[CONV1_FEAT_SIZE][CONV1_KERN_SIZE][CONV1_KERN_SIZE];
extern int32_t out_conv1[CONV1_OUT_SIZE][CONV1_OUT_SIZE][CONV1_FEAT_SIZE];

#define MAXPOOL1_IN_SIZE CONV1_OUT_SIZE          // 24
#define MAXPOOL1_FEAT_SIZE CONV1_FEAT_SIZE       // 6
#define MAXPOOL1_OUT_SIZE (MAXPOOL1_IN_SIZE / 2) // 12

extern int32_t out_max1[MAXPOOL1_OUT_SIZE][MAXPOOL1_OUT_SIZE][MAXPOOL1_FEAT_SIZE];

#define CONV2_IN_SIZE MAXPOOL1_OUT_SIZE // 12
#define CONV2_KERN_SIZE 5
#define CONV2_FEAT_IN_SIZE MAXPOOL1_FEAT_SIZE // 6
#define CONV2_FEAT_OUT_SIZE 16
#define CONV2_OUT_SIZE (CONV2_IN_SIZE - CONV2_KERN_SIZE + 1) // 8

extern int32_t weights_conv2[CONV2_KERN_SIZE][CONV2_KERN_SIZE][CONV2_FEAT_OUT_SIZE][CONV2_FEAT_IN_SIZE];
extern int32_t out_conv2[CONV2_OUT_SIZE][CONV2_OUT_SIZE][CONV2_FEAT_OUT_SIZE];

#define MAXPOOL2_IN_SIZE CONV2_OUT_SIZE          // 8
#define MAXPOOL2_FEAT_SIZE CONV2_FEAT_OUT_SIZE   // 16
#define MAXPOOL2_OUT_SIZE (MAXPOOL2_IN_SIZE / 2) // 4

extern int32_t out_max2[MAXPOOL2_OUT_SIZE][MAXPOOL2_OUT_SIZE][MAXPOOL2_FEAT_SIZE];

#define FC1_IN_SIZE (MAXPOOL2_OUT_SIZE * MAXPOOL2_OUT_SIZE * MAXPOOL2_FEAT_SIZE) // 256
#define FC1_OUT_SIZE 120

extern int32_t fc1[FC1_OUT_SIZE][FC1_IN_SIZE];
extern int32_t out_fc1[FC1_OUT_SIZE];

#define FC2_IN_SIZE FC1_OUT_SIZE // 120
#define FC2_OUT_SIZE 84

extern int32_t fc2[FC2_OUT_SIZE][FC2_IN_SIZE];
extern int32_t out_fc2[FC2_OUT_SIZE];

#define FC3_IN_SIZE FC2_OUT_SIZE // 84
#define FC3_OUT_SIZE 10

extern int32_t fc3[FC3_OUT_SIZE][FC3_IN_SIZE];
extern volatile int32_t out_fc3[FC3_OUT_SIZE];

int main()
{
    puts("");
    // convolution layer 1
    for (int f = 0; f < CONV1_FEAT_SIZE; ++f)
    {
        for (int i1 = 0; i1 < CONV1_OUT_SIZE; i1++)
        {
            for (int j1 = 0; j1 < CONV1_OUT_SIZE; ++j1)
            {
                int32_t sum = 0;
                for (int i2 = 0; i2 < CONV1_KERN_SIZE; ++i2)
                {
                    for (int j2 = 0; j2 < CONV1_KERN_SIZE; ++j2)
                    {
                        sum += in1_conv1[i1 + i2][j1 + j2] * weights_conv1[f][i2][j2];
                    }
                }
                out_conv1[i1][j1][f] = sum >> 3;
            }
        }
    }

    // maxpooling layer 1
    for (int i1 = 0; i1 < MAXPOOL1_OUT_SIZE; i1++)
    {
        for (int j1 = 0; j1 < MAXPOOL1_OUT_SIZE; ++j1)
        {
            for (int f = 0; f < MAXPOOL1_FEAT_SIZE; ++f)
            {
                out_max1[i1][j1][f] = INT32_MIN;
                for (int i2 = 0; i2 < 2; ++i2)
                {
                    for (int j2 = 0; j2 < 2; ++j2)
                    {
                        out_max1[i1][j1][f] = MAX(out_max1[i1][j1][f], out_conv1[2 * i1 + i2][2 * j1 + j2][f]);
                    }
                }
            }
        }
    }

    // convolution layer 2
    for (int i1 = 0; i1 < CONV2_OUT_SIZE; ++i1)
    {
        for (int j1 = 0; j1 < CONV2_OUT_SIZE; ++j1)
        {
            for (int f = 0; f < CONV2_FEAT_OUT_SIZE; ++f)
            {
                int32_t sum = 0;
                for (int m = 0; m < CONV2_FEAT_IN_SIZE; ++m)
                {
                    for (int i2 = 0; i2 < CONV2_KERN_SIZE; ++i2)
                    {
                        for (int j2 = 0; j2 < CONV2_KERN_SIZE; ++j2)
                        {
                            sum += out_max1[i1 + i2][j1 + j2][m] * weights_conv2[i2][j2][f][m];
                        }
                    }
                }
                out_conv2[i1][j1][f] = sum >> 3;
            }
        }
    }

    // maxpooling layer 2
    for (int i1 = 0; i1 < MAXPOOL2_OUT_SIZE; i1++)
    {
        for (int j1 = 0; j1 < MAXPOOL2_OUT_SIZE; ++j1)
        {
            for (int f = 0; f < MAXPOOL2_FEAT_SIZE; ++f)
            {
                out_max2[i1][j1][f] = INT32_MIN;
                for (int i2 = 0; i2 < 2; ++i2)
                {
                    for (int j2 = 0; j2 < 2; ++j2)
                    {

                        out_max2[i1][j1][f] = MAX(out_max2[i1][j1][f], out_conv2[2 * i1 + i2][2 * j1 + j2][f]);
                    }
                }
            }
        }
    }

    // flatten
    int32_t *flatten = (int32_t *)&out_max2;

    // fully connected layer 1
    for (int i1 = 0; i1 < FC1_OUT_SIZE; ++i1)
    {
        int32_t sum = 0;
        for (int f = 0; f < FC1_IN_SIZE; ++f)
        {
            const int32_t wgt_pix = flatten[f] * fc1[i1][f];
            sum += wgt_pix;
        }
        sum = sum >> 3;
        out_fc1[i1] = MAX(sum, 0);
    }

    // fully connected layer 2
    for (int i1 = 0; i1 < FC2_OUT_SIZE; ++i1)
    {
        int32_t sum = 0;
        for (int f = 0; f < FC2_IN_SIZE; ++f)
        {
            sum += out_fc1[f] * fc2[i1][f];
        }
        sum = sum >> 3;
        out_fc2[i1] = MAX(sum, 0);
    }

    // fully connected layer 3
    for (int i1 = 0; i1 < FC3_OUT_SIZE; ++i1)
    {
        int32_t sum = 0;
        for (int f = 0; f < FC3_IN_SIZE; ++f)
        {
            sum += out_fc2[f] * fc3[i1][f];
        }
        sum = sum >> 3;
        out_fc3[i1] = MAX(sum, 0);
    }
    puts("");
    return 0;
}
