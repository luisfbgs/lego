#!/bin/bash

make
./translator triangulo
nasm -f elf64 -o triangulo.o triangulo.s
ld -o triangulo triangulo.o
 
