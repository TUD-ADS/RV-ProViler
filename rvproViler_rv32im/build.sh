set -e

if [ -d "./out" ] ; then
    rm -rf ./out
fi

mkdir out
cd out

declare -a BM_LIST=("matmul256")

for bm in ${BM_LIST[@]};
do
    riscv32-unknown-elf-gcc         -march=rv32i -O2 ../${bm}_base.c ../${bm}_data_base.c -o ${bm}_base
    riscv32-unknown-elf-objdump     -d ${bm}_base > ${bm}_base.dump
done

cd ..
bash "./gen_hist.sh"