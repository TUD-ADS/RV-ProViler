set -e

if [ -d "./out" ] ; then
    rm -rf ./out
fi

mkdir out
cd out

declare -a BM_LIST=("matmul256" "conv256" "fc1024" "maxpooling256" "mlenet5")
declare -a CONF_LIST=("v_vec8" "v_vec16" "v_vec32")

for bm in ${BM_LIST[@]};
do
    for conf in ${CONF_LIST[@]};
    do
        riscv32-unknown-elf-gcc         -march=rv32gcv -O2 ../${bm}_${conf}.c ../${bm}_data_${conf}.c -o ${bm}_${conf}
        riscv32-unknown-elf-objdump     -d ${bm}_${conf} > ${bm}_${conf}.dump
    done
    
    riscv32-unknown-elf-gcc         -march=rv32gc -O2 ../${bm}_base.c ../${bm}_data_base.c -o ${bm}_base
    riscv32-unknown-elf-objdump     -d ${bm}_base > ${bm}_base.dump
done

cd ..
bash "./gen_hist.sh"