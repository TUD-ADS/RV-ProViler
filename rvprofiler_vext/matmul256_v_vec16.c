#include "stdint.h"
#include "stdio.h"
#include "common.h"
#include "riscv_vector.h"

extern int16_t in1[MATMUL_SIZE][MATMUL_SIZE];
extern int16_t in2[MATMUL_SIZE][MATMUL_SIZE];
extern volatile int16_t out1[MATMUL_SIZE][MATMUL_SIZE];



int main() {

  puts("");

    size_t vlmax = __riscv_vsetvlmax_e16m1();

    for (int i = 0; i < MATMUL_SIZE; ++i) {
      for (int j = 0; j < MATMUL_SIZE; ++j) {
        int16_t *ptr_a = &in1[i][0];
        int16_t *ptr_b = &in2[j][0];
        int k = MATMUL_SIZE;
        vint16m1_t vec_s = __riscv_vmv_v_x_i16m1(0, vlmax);
        vint16m1_t vec_zero = __riscv_vmv_v_x_i16m1(0, vlmax);
        for (size_t vl; k > 0; k -= vl, ptr_a += vl, ptr_b += vl) {
          vl = __riscv_vsetvl_e16m1(k);

          vint16m1_t vec_a = __riscv_vle16_v_i16m1(ptr_a, vl);
          vint16m1_t vec_b = __riscv_vle16_v_i16m1(ptr_b, vl);

          vec_s = __riscv_vmacc_vv_i16m1(vec_s, vec_a, vec_b, vl);
        }

        vint16m1_t vec_sum;
        vec_sum = __riscv_vredsum_vs_i16m1_i16m1(vec_s, vec_zero, vlmax);
        int16_t sum = __riscv_vmv_x_s_i16m1_i16(vec_sum);
        out1[i][j] = sum;
      }
    }

  puts("");


  return 0;
}
