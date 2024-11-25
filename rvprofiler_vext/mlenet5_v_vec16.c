#include "stdint.h"
#include "stdio.h"
#include "riscv_vector.h"

#define MAX(x, y) (((x) > (y)) ? (x) : (y))

#define CONV1_IN_SIZE 28
#define CONV1_KERN_SIZE 5
#define CONV1_FEAT_SIZE 6
#define CONV1_OUT_SIZE (CONV1_IN_SIZE - CONV1_KERN_SIZE + 1) // 24

extern int16_t in1_conv1[CONV1_IN_SIZE][CONV1_IN_SIZE + 3];
extern int16_t weights_conv1[CONV1_FEAT_SIZE][CONV1_KERN_SIZE * (CONV1_KERN_SIZE + 3)];
extern int16_t out_conv1[CONV1_OUT_SIZE][CONV1_OUT_SIZE][CONV1_FEAT_SIZE];

#define MAXPOOL1_IN_SIZE CONV1_OUT_SIZE          // 24
#define MAXPOOL1_FEAT_SIZE CONV1_FEAT_SIZE       // 6
#define MAXPOOL1_OUT_SIZE (MAXPOOL1_IN_SIZE / 2) // 12

extern int16_t out_max1[MAXPOOL1_OUT_SIZE][MAXPOOL1_OUT_SIZE][MAXPOOL1_FEAT_SIZE+2];

#define CONV2_IN_SIZE MAXPOOL1_OUT_SIZE // 12
#define CONV2_KERN_SIZE 5
#define CONV2_FEAT_IN_SIZE MAXPOOL1_FEAT_SIZE // 6
#define CONV2_FEAT_OUT_SIZE 16
#define CONV2_OUT_SIZE (CONV2_IN_SIZE - CONV2_KERN_SIZE + 1) // 8

extern int16_t weights_conv2[CONV2_KERN_SIZE * CONV2_KERN_SIZE][CONV2_FEAT_OUT_SIZE][CONV2_FEAT_IN_SIZE+2];
extern int16_t out_conv2[CONV2_OUT_SIZE][CONV2_OUT_SIZE][CONV2_FEAT_OUT_SIZE];

#define MAXPOOL2_IN_SIZE CONV2_OUT_SIZE          // 8
#define MAXPOOL2_FEAT_SIZE CONV2_FEAT_OUT_SIZE   // 16
#define MAXPOOL2_OUT_SIZE (MAXPOOL2_IN_SIZE / 2) // 4

extern int16_t out_max2[MAXPOOL2_OUT_SIZE][MAXPOOL2_OUT_SIZE][MAXPOOL2_FEAT_SIZE];

#define FC1_IN_SIZE (MAXPOOL2_OUT_SIZE * MAXPOOL2_OUT_SIZE * MAXPOOL2_FEAT_SIZE) // 256
#define FC1_OUT_SIZE 120

extern int16_t fc1[FC1_OUT_SIZE][FC1_IN_SIZE];
extern int16_t out_fc1[FC1_OUT_SIZE];

#define FC2_IN_SIZE FC1_OUT_SIZE // 120
#define FC2_OUT_SIZE 84

extern int16_t fc2[FC2_OUT_SIZE][FC2_IN_SIZE];
extern int16_t out_fc2[FC2_OUT_SIZE];

#define FC3_IN_SIZE FC2_OUT_SIZE // 84
#define FC3_OUT_SIZE 10

extern int16_t fc3[FC3_OUT_SIZE][FC3_IN_SIZE];
extern volatile int16_t out_fc3[FC3_OUT_SIZE];

int main()
{
    puts("");

    size_t vlmax = __riscv_vsetvlmax_e16m1();

    // convolution layer 1

    for (int f = 0; f < CONV1_FEAT_SIZE; ++f) {
        for (int i1 = 0; i1 < CONV1_OUT_SIZE; ++i1) {
            for (int j1 = 0; j1 < CONV1_OUT_SIZE; ++j1) {
                int16_t sum = 0;
                vint16m1_t vec_zero = __riscv_vmv_v_x_i16m1(0, vlmax);

                vint16m1_t vec_sum = __riscv_vmv_v_x_i16m1(0, vlmax);
                
                int16_t *ptr_a = &in1_conv1[i1][j1];
                int16_t *ptr_b = &weights_conv1[f][0];
                int j2 = CONV1_KERN_SIZE * CONV1_KERN_SIZE;

                for (size_t vl; j2 > 0; j2 -= vl, ptr_a += vl, ptr_b += vl) {
                    vl = __riscv_vsetvl_e16m1(j2);
                        
                    vint16m1_t vec_a = __riscv_vle16_v_i16m1(ptr_a, vl);
                    vint16m1_t vec_b = __riscv_vle16_v_i16m1(ptr_b, vl);
                        
                    // Perform element-wise multiplication and accumulation
                    vec_sum = __riscv_vmacc_vv_i16m1(vec_sum, vec_a, vec_b, vl);
                }
                
                // Perform reduction to compute the sum across the vector elements
                vec_sum = __riscv_vredsum_vs_i16m1_i16m1(vec_sum, vec_zero, vlmax);

                // Extract the sum from the vector and store it in the output array
                sum = __riscv_vmv_x_s_i16m1_i16(vec_sum);
                out_conv1[i1][j1][f] = sum >> 3;
            
            }
        }
    }

    // maxpooling layer 1
    for (int i1 = 0; i1 < MAXPOOL1_OUT_SIZE; i1++) {
        for (int j1 = 0; j1 < MAXPOOL1_OUT_SIZE; j1++) {
            for (int f = 0; f < MAXPOOL1_FEAT_SIZE; f += vlmax) {
                
                size_t vl = __riscv_vsetvl_e16m1(MAXPOOL1_FEAT_SIZE - f);
                vint16m1_t max_val_vec = __riscv_vmv_v_x_i16m1(INT16_MIN, vl);
                
                for (int i2 = 0; i2 < 2; i2++) {
                    for (int j2 = 0; j2 < 2; j2++) {
                        vint16m1_t input_val_vec = __riscv_vle16_v_i16m1(&out_conv1[2 * i1 + i2][2 * j1 + j2][f], vl);
                        max_val_vec = __riscv_vmax_vv_i16m1(max_val_vec, input_val_vec, vl);
                    }
                }

                // Store the result
                __riscv_vse16_v_i16m1((int16_t *)&out_max1[i1][j1][f], max_val_vec, vl);

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
                int16_t sum = 0;
                vint16m1_t vec_zero = __riscv_vmv_v_x_i16m1(0, vlmax);
                vint16m1_t vec_sum = __riscv_vmv_v_x_i16m1(0, vlmax);
                for (int m = 0; m < CONV2_FEAT_IN_SIZE; ++m)
                {
                    int16_t *ptr_a = &out_max1[i1][j1][m];
                    int16_t *ptr_b = &weights_conv2[0][f][m];
                    int j2 = CONV2_KERN_SIZE * CONV2_KERN_SIZE;

                    for (size_t vl; j2 > 0; j2 -= vl, ptr_a += vl, ptr_b += vl) 
                    {
                        vl = __riscv_vsetvl_e16m1(j2);
                        
                        vint16m1_t vec_a = __riscv_vle16_v_i16m1(ptr_a, vl);
                        vint16m1_t vec_b = __riscv_vle16_v_i16m1(ptr_b, vl);
                        
                        // Perform element-wise multiplication and accumulation
                        vec_sum = __riscv_vmacc_vv_i16m1(vec_sum, vec_a, vec_b, vl);
                    }
                }
                // Perform reduction to compute the sum across the vector elements
                vec_sum = __riscv_vredsum_vs_i16m1_i16m1(vec_sum, vec_zero, vlmax);

                // Extract the sum from the vector and store it in the output array
                sum = __riscv_vmv_x_s_i16m1_i16(vec_sum);
                out_conv2[i1][j1][f] = sum >>3;
            }

        }
    }
    
    // maxpooling layer 2
    for (int i1 = 0; i1 < MAXPOOL2_OUT_SIZE; i1++) {
        for (int j1 = 0; j1 < MAXPOOL2_OUT_SIZE; j1++) {
            for (int f = 0; f < MAXPOOL2_FEAT_SIZE; f += vlmax) {
                
                size_t vl = __riscv_vsetvl_e16m1(MAXPOOL2_FEAT_SIZE - f);
                vint16m1_t max_val_vec = __riscv_vmv_v_x_i16m1(INT16_MIN, vl);
                
                for (int i2 = 0; i2 < 2; i2++) {
                    for (int j2 = 0; j2 < 2; j2++) {
                        vint16m1_t input_val_vec = __riscv_vle16_v_i16m1(&out_conv2[2 * i1 + i2][2 * j1 + j2][f], vl);
                        max_val_vec = __riscv_vmax_vv_i16m1(max_val_vec, input_val_vec, vl);
                    }
                }

                // Store the result
                __riscv_vse16_v_i16m1((int16_t *)&out_max2[i1][j1][f], max_val_vec, vl);

            }
        }
    }

    // flatten
    int16_t *flatten = (int16_t *)&out_max2;

    // fully connected layer 1
    for (int i1 = 0; i1 < FC1_OUT_SIZE; ++i1) {
        int16_t *ptr_a = &flatten[0];
        int16_t *ptr_b = &fc1[i1][0];
        int f = FC1_IN_SIZE;
        vint16m1_t vec_s = __riscv_vmv_v_x_i16m1(0, vlmax);
        vint16m1_t vec_zero = __riscv_vmv_v_x_i16m1(0, vlmax);
        int32_t sum = 0;

        for (size_t vl; f > 0; f -= vl, ptr_a += vl, ptr_b += vl) {
            vl = __riscv_vsetvl_e16m1(f);

            vint16m1_t vec_a = __riscv_vle16_v_i16m1(ptr_a, vl);
            vint16m1_t vec_b = __riscv_vle16_v_i16m1(ptr_b, vl);

            vec_s = __riscv_vmacc_vv_i16m1(vec_s, vec_a, vec_b, vl); // Element-wise MAC

        }
        // Horizontal reduction (sum)
        vint16m1_t vec_sum = __riscv_vredsum_vs_i16m1_i16m1(vec_s, vec_zero, vlmax);
        sum += __riscv_vmv_x_s_i16m1_i16(vec_sum); 
        out_fc1[i1] = MAX(sum, 0);
    }

    // fully connected layer 2
    for (int i1 = 0; i1 < FC2_OUT_SIZE; ++i1) {
        int16_t *ptr_a = &out_fc1[0];
        int16_t *ptr_b = &fc2[i1][0];
        int f = FC2_IN_SIZE;
        vint16m1_t vec_s = __riscv_vmv_v_x_i16m1(0, vlmax);
        vint16m1_t vec_zero = __riscv_vmv_v_x_i16m1(0, vlmax);
        int32_t sum = 0;

        for (size_t vl; f > 0; f -= vl, ptr_a += vl, ptr_b += vl) {
            vl = __riscv_vsetvl_e16m1(f);

            vint16m1_t vec_a = __riscv_vle16_v_i16m1(ptr_a, vl);
            vint16m1_t vec_b = __riscv_vle16_v_i16m1(ptr_b, vl);

            vec_s = __riscv_vmacc_vv_i16m1(vec_s, vec_a, vec_b, vl); // Element-wise MAC

        }
        // Horizontal reduction (sum)
        vint16m1_t vec_sum = __riscv_vredsum_vs_i16m1_i16m1(vec_s, vec_zero, vlmax);
        sum += __riscv_vmv_x_s_i16m1_i16(vec_sum); 
        out_fc2[i1] = MAX(sum, 0);
    }

    // fully connected layer 3
    for (int i1 = 0; i1 < FC3_OUT_SIZE; ++i1) {
        int16_t *ptr_a = &out_fc2[0];
        int16_t *ptr_b = &fc3[i1][0];
        int f = FC3_IN_SIZE;
        vint16m1_t vec_s = __riscv_vmv_v_x_i16m1(0, vlmax);
        vint16m1_t vec_zero = __riscv_vmv_v_x_i16m1(0, vlmax);
        int32_t sum = 0;

        for (size_t vl; f > 0; f -= vl, ptr_a += vl, ptr_b += vl) {
            vl = __riscv_vsetvl_e16m1(f);

            vint16m1_t vec_a = __riscv_vle16_v_i16m1(ptr_a, vl);
            vint16m1_t vec_b = __riscv_vle16_v_i16m1(ptr_b, vl);

            vec_s = __riscv_vmacc_vv_i16m1(vec_s, vec_a, vec_b, vl); // Element-wise MAC

        }
        // Horizontal reduction (sum)
        vint16m1_t vec_sum = __riscv_vredsum_vs_i16m1_i16m1(vec_s, vec_zero, vlmax);
        sum += __riscv_vmv_x_s_i16m1_i16(vec_sum); 
        out_fc2[i1] = MAX(sum, 0);
    }
    puts("");
    return 0;
}
