# RV-ProViler


# Dependencies

1. OS: Ubuntu
2. Python
3. RISC-V GNU Compiler Toolchain
	
* dependencies:

>
>export RISCV=/opt/riscv

>export PATH="$PATH:$RISCV/bin"

>sudo apt-get install autoconf automake autotools-dev curl python3 libmpc-dev libmpfr-dev libgmp-dev gawk build-essential bison flex texinfo gperf libtool patchutils bc zlib1g-dev libexpat-dev device-tree-compiler


* RV32IMV

gcc:
>
>git clone https://github.com/riscv-collab/riscv-gnu-toolchain

>cd riscv-gnu-toolchain

>./configure --prefix=$RISCV --with-arch=rv32imc_zifencei --with-abi=ilp32 --enable-multilib

>In makefile set INSTALL_DIR := /opt/riscv

>sudo make

pk:
>
>git clone https://github.com/riscv-software-src/riscv-pk

>rm -rf build && mkdir build && cd build

>../configure --prefix=$RISCV --host=riscv32-unknown-elf --with-arch=rv32gc_zifencei

>make

>sudo make install

spike:
>
>git clone https://github.com/riscv-software-src/riscv-isa-sim

>rm -rf build && mkdir build && cd build

>../configure --prefix=$RISCV --with-isa=rv32gc_zifencei --enable-histogram

>make

>sudo make install

# Usage


1. Make sure RISCV is in your path

2. run "build.sh"

This will generate the following files:
    - objdump files in /out
    - ISA coverage charts in /plots
    - ISA coverage in csv format in /csv

- Adapt function inputs and kernel sizes in common.h
