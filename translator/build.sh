#!/bin/bash

make
./translator triangulo > a.asm
nasm -f elf64 -o a.o a.asm
ld -o a a.o
 
