#!/bin/sh
set -e
sed 's/^[cinv][[:lower:]]*/static &/;s/static int main/int main/' [0-9]*.c >stage1.c
clang -Weverything -o stage1 stage1.c
