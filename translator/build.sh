#!/bin/bash

make
rm triangulo triangulo.o triangulo.s
./translator triangulo
nasm -f elf -o triangulo.o triangulo.s
ld -m elf_i386 -o triangulo triangulo.o

