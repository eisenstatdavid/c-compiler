#!/bin/sh
grep -E '^[cinv][[:lower:]]* |internalerror' [0-9]*.c | grep -B1 internalerror
echo '########'
grep -E -A10 ' >= MAX|oom' [0-9]*.c | grep -B1 oom
