#include "stdint.h"
#include "stdio.h"
#include "common.h"
#include "riscv_vector.h"

extern int8_t in1[FC_IN_SIZE];
extern int8_t wgts[FC_OUT_SIZE][FC_IN_SIZE];
extern volatile int8_t out1[FC_OUT_SIZE];

int main(){

    puts("");

    size_t vlmax = __riscv_vsetvlmax_e8m1();

    for (int i1 = 0; i1 < FC_OUT_SIZE; ++i1) {
        int8_t *ptr_a = &in1[0];
        int8_t *ptr_b = &wgts[i1][0];
        int f = FC_IN_SIZE;
        vint8m1_t vec_s = __riscv_vmv_v_x_i8m1(0, vlmax);
        vint8m1_t vec_zero = __riscv_vmv_v_x_i8m1(0, vlmax);
        int32_t sum = 0;

        for (size_t vl; f > 0; f -= vl, ptr_a += vl, ptr_b += vl) {
            vl = __riscv_vsetvl_e8m1(f);

            vint8m1_t vec_a = __riscv_vle8_v_i8m1(ptr_a, vl);
            vint8m1_t vec_b = __riscv_vle8_v_i8m1(ptr_b, vl);

            vec_s = __riscv_vmacc_vv_i8m1(vec_s, vec_a, vec_b, vl); // Element-wise MAC

        }
        // Horizontal reduction (sum)
        vint8m1_t vec_sum = __riscv_vredsum_vs_i8m1_i8m1(vec_s, vec_zero, vlmax);
        sum += __riscv_vmv_x_s_i8m1_i8(vec_sum); 
        out1[i1] = sum;
    }
    

    puts("");

    return 0;
}