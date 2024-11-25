#include "stdint.h"
#include "stdio.h"
#include "common.h"
#include "riscv_vector.h"

extern int32_t in1[MATMUL_SIZE][MATMUL_SIZE];
extern int32_t in2[MATMUL_SIZE][MATMUL_SIZE];
extern volatile int32_t out1[MATMUL_SIZE][MATMUL_SIZE];



int main() {

  puts("");

    size_t vlmax = __riscv_vsetvlmax_e32m1();

    for (int i = 0; i < MATMUL_SIZE; ++i) {
      for (int j = 0; j < MATMUL_SIZE; ++j) {
        int32_t *ptr_a = &in1[i][0];
        int32_t *ptr_b = &in2[j][0];
        int k = MATMUL_SIZE;
        vint32m1_t vec_s = __riscv_vmv_v_x_i32m1(0, vlmax);
        vint32m1_t vec_zero = __riscv_vmv_v_x_i32m1(0, vlmax);
        for (size_t vl; k > 0; k -= vl, ptr_a += vl, ptr_b += vl) {
          vl = __riscv_vsetvl_e32m1(k);

          vint32m1_t vec_a = __riscv_vle32_v_i32m1(ptr_a, vl);
          vint32m1_t vec_b = __riscv_vle32_v_i32m1(ptr_b, vl);

          vec_s = __riscv_vmacc_vv_i32m1(vec_s, vec_a, vec_b, vl);
        }

        vint32m1_t vec_sum;
        vec_sum = __riscv_vredsum_vs_i32m1_i32m1(vec_s, vec_zero, vlmax);
        int32_t sum = __riscv_vmv_x_s_i32m1_i32(vec_sum);
        out1[i][j] = sum;
      }
    }

  puts("");


  return 0;
}
