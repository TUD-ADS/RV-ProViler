#include "stdint.h"
#include "stdio.h"
#include "common.h"
#include "riscv_vector.h"

extern int32_t in1[MAXPOOL_IN_SIZE][MAXPOOL_IN_SIZE][MAXPOOL_FEAT_SIZE];
extern volatile int32_t out1[MAXPOOL_OUT_SIZE][MAXPOOL_OUT_SIZE][MAXPOOL_FEAT_SIZE];

int main()
{
    puts("");
   
    const size_t vlmax = __riscv_vsetvlmax_e32m1();
    
    for (int i1 = 0; i1 < MAXPOOL_OUT_SIZE; i1++) {
        for (int j1 = 0; j1 < MAXPOOL_OUT_SIZE; j1++) {
            for (int f = 0; f < MAXPOOL_FEAT_SIZE; f += vlmax) {
                
                size_t vl = __riscv_vsetvl_e32m1(MAXPOOL_FEAT_SIZE - f);
                vint32m1_t max_val_vec = __riscv_vmv_v_x_i32m1(INT32_MIN, vl);
                
                for (int i2 = 0; i2 < 2; i2++) {
                    for (int j2 = 0; j2 < 2; j2++) {
                        vint32m1_t input_val_vec = __riscv_vle32_v_i32m1(&in1[2 * i1 + i2][2 * j1 + j2][f], vl);
                        max_val_vec = __riscv_vmax_vv_i32m1(max_val_vec, input_val_vec, vl);
                    }
                }

                // Store the result
                __riscv_vse32_v_i32m1((int32_t *)&out1[i1][j1][f], max_val_vec, vl);

            }
        }
    }

    puts("");

    return 0;
}
