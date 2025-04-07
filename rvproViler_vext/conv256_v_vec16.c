#include "stdint.h"
#include "stdio.h"
#include "common.h"
#include "riscv_vector.h"

extern int16_t in1[CONV_IN_SIZE][CONV_IN_SIZE];
extern int16_t wgts1[CONV_FEAT_SIZE][CONV_KERN_SIZE * CONV_KERN_SIZE];
extern volatile int16_t out1[CONV_OUT_SIZE][CONV_OUT_SIZE][CONV_FEAT_SIZE];

int main()
{
    puts("");

    // Kernel unrolled

    size_t vlmax = __riscv_vsetvlmax_e8m1();

    for (int f = 0; f < CONV_FEAT_SIZE; ++f) 
    {
        for (int i1 = 0; i1 < CONV_OUT_SIZE; ++i1) 
        {
            for (int j1 = 0; j1 < CONV_OUT_SIZE; ++j1) 
            {
                int16_t sum = 0;
                vint16m1_t vec_zero = __riscv_vmv_v_x_i16m1(0, vlmax);

                vint16m1_t vec_sum = __riscv_vmv_v_x_i16m1(0, vlmax);
                
                // Iterate over the kernel

                int16_t *ptr_a = &in1[i1][j1];
                int16_t *ptr_b = &wgts1[f][0];
                int j2 = CONV_KERN_SIZE * CONV_KERN_SIZE;

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
                out1[i1][j1][f] = sum;
            
            }
        }
    }
    puts("");

    return 0;
}


/*

    // Kernel not unrolled

    size_t vlmax = __riscv_vsetvlmax_e8m1();

    for (int f = 0; f < CONV_FEAT_SIZE; ++f) {
        for (int i1 = 0; i1 < CONV_OUT_SIZE; ++i1) {
            for (int j1 = 0; j1 < CONV_OUT_SIZE; ++j1) {
                int8_t sum = 0;
                vint8m1_t vec_zero = __riscv_vmv_v_x_i8m1(0, vlmax);

                vint8m1_t vec_sum = __riscv_vmv_v_x_i8m1(0, vlmax);
                
                // Iterate over the kernel
                for (int i2 = 0; i2 < CONV_KERN_SIZE; ++i2) {
                    
                    int8_t *ptr_a = &in1[i1 + i2][j1];
                    int8_t *ptr_b = &wgts1[f][i2][0];
                    int j2 = CONV_KERN_SIZE;

                    for (size_t vl; j2 > 0; j2 -= vl, ptr_a += vl, ptr_b += vl) {
                        vl = __riscv_vsetvl_e8m1(j2);
                        
                        vint8m1_t vec_a = __riscv_vle8_v_i8m1(ptr_a, vl);
                        vint8m1_t vec_b = __riscv_vle8_v_i8m1(ptr_b, vl);
                        
                        // Perform element-wise multiplication and accumulation
                        vec_sum = __riscv_vmacc_vv_i8m1(vec_sum, vec_a, vec_b, vl);
                    }
                }
                
                // Perform reduction to compute the sum across the vector elements
                vec_sum = __riscv_vredsum_vs_i8m1_i8m1(vec_sum, vec_zero, vlmax);

                // Extract the sum from the vector and store it in the output array
                sum = __riscv_vmv_x_s_i8m1_i8(vec_sum);
                out1[i1][j1][f] = sum;
            
            }
        }
    }

*/