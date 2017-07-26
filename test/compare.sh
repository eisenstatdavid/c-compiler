#!/bin/sh
set -e
for f in test/*.c; do
cat 0prim.c 1stdlib.c "${f}" >in.c
"$1" -std=c99 -funsigned-char -o a in.c
./a >out.a
cat 1stdlib.c "${f}" | "$2" >b.nasm
{ nasm -f macho -o b.o b.nasm && ld -o b b.o; } || { nasm -f elf -o b.o b.nasm && ld -o b b.o; }
./b >out.b
cmp out.a out.b
done
