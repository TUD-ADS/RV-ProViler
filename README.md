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


* RV32IMP

gcc:

>
>git clone https://github.com/riscv-collab/riscv-gnu-toolchain
>cd riscv-gnu-toolchain
>git clone https://github.com/plctlab/riscv-gcc -b riscv-gcc-p-ext riscv-gcc-p-ext
>git clone https://github.com/plctlab/riscv-binutils-gdb -b riscv-binutils-p-ext riscv-binutils-p-ext
>./configure --prefix=$RISCV --with-arch=rv32imap_zifencei --with-abi=ilp32 --with-gcc-src=`pwd`/riscv-gcc-p-ext --with-binutils-src=`pwd`/riscv-binutils-p-ext
># In makefile set INSTALL_DIR := /opt/riscv
>sudo make

pk:
>
>git clone https://github.com/riscv-software-src/riscv-pk
>rm -rf build && mkdir build && cd build
>../configure --prefix=$RISCV --host=riscv32-unknown-elf --with-arch=rv32gcp_zifencei
>make
>sudo make install

spike:
>
>git clone https://github.com/riscv-software-src/riscv-isa-sim
>rm -rf build && mkdir build && cd build
>../configure --prefix=$RISCV --with-isa=rv32gcp_zifencei --enable-histogram
>make
>sudo make install


* RV32IMV

gcc:
>
>git clone https://github.com/riscv-collab/riscv-gnu-toolchain
>cd riscv-gnu-toolchain
>./configure --prefix=$RISCV --with-arch=rv32imav_zifencei --with-abi=ilp32 --enable-multilib
># In makefile set INSTALL_DIR := /opt/riscv
>sudo make

pk:
>
>git clone https://github.com/riscv-software-src/riscv-pk
>rm -rf build && mkdir build && cd build
>../configure --prefix=$RISCV --host=riscv32-unknown-elf --with-arch=rv32gcv_zifencei
>make
>sudo make install

spike:
>
>git clone https://github.com/riscv-software-src/riscv-isa-sim
>rm -rf build && mkdir build && cd build
>../configure --prefix=$RISCV --with-isa=rv32gcv_zifencei --enable-histogram
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

---------------------------------------------------------------------------------------------------

# Citation
If you use this code as part of your work, please cite the following paper:

M. Ali and E. Aliagha, M. Elnashar and D. Göhringer, RV-ProViler: Evaluating RISC-V ISA for Application-Specific Requirements, In IEEE Nordic Circuits and Systems Conference (NorCAS), October 2024.

# Contact Info
M.Sc. Muhammad Ali, Technische Universität Dresden, muhammad.ali@tu-dresden.de,

Google Scholar: https://scholar.google.com/citations?hl=en&user=lQDppZ8AAAAJ

# Acknowledgment
This work has been partially funded by the German Federal Ministry of Education and Research BMBF as part of the DAKORE (Datenfunknetz mit Adaptivhardware und KI-Optimierung zur Reduktion des Energieverbrauchs) project under grant agreement number 16ME0433K and by the German Research Foundation (“Deutsche Forschungsgemeinschaft”) (DFG) under Project-ID 287022738 TRR 196 for Project S05.
