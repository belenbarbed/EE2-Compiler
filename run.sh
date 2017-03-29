#!/bin/bash

# make 'n' shit
make clean
clear
make bin/c_compiler

echo ""

# print out compiled assembly in terminal
./bin/c_compiler<test.c

# saved compiled assembly in .s file
./bin/c_compiler<test.c > main.s

# execute my generated assembly
mips-linux-gnu-gcc -static -Wall main.s -o a.out
qemu-mips a.out

# gcc compiles same code
mips-linux-gnu-gcc -c -S test.c
