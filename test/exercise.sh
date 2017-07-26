#!/bin/sh
for f in test/badlex/*.c; do "$1" <"${f}"; done
python3 test/testsafe.py "$1"
for f in test/badparse/*.c; do "$1" <"${f}"; done
cat 1stdlib.c 2lex.c 3safe.c 4parse.c | "$1"
for f in test/*.c; do cat 1stdlib.c "${f}" | "$1"; done
