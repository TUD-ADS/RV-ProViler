#!/bin/bash

set -e

declare -a BM_LIST=("matmul256" "fc1024" "maxpooling256" "conv256" "mlenet5")
declare -a CONF_LIST=("base" "p_simd8" "p_simd16")

for bm in ${BM_LIST[@]};
do
    for conf in ${CONF_LIST[@]};
    do
        echo "-- Running: ${bm}_${conf}"
        spike   --misaligned                            \
                -g                                      \
                --isa=rv32gcp_zifencei                  \
                $RISCV/riscv32-unknown-elf/bin/pk       \
                ./out/${bm}_${conf}                     \
                > /dev/null 2> "./out/${bm}_${conf}.his"
    done
done

echo "Histogram files created"

cd scripts
python3 filter_his.py
python3 create_data.py
python3 plot_summary.py
