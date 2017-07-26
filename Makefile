bootstrap: stage2.nasm stage3.nasm
	cmp stage[23].nasm

.PHONY: bootstrap

stage1.c: 0prim.c 1stdlib.c 2lex.c 3safe.c 4parse.c
	cat >$@ 0prim.c 1stdlib.c 2lex.c 3safe.c 4parse.c

stage1: stage1.c
	$(CC) -std=c99 -o $@ $<

stage2.c: 1stdlib.c 2lex.c 3safe.c 4parse.c
	cat >$@ 1stdlib.c 2lex.c 3safe.c 4parse.c

stage2.nasm: stage1 stage2.c
	./stage1 >$@ <stage2.c

stage2: stage2.nasm
	{ nasm -f macho -o stage2.o stage2.nasm && ld -o $@ stage2.o; } || { nasm -f elf -o stage2.o stage2.nasm && ld -o $@ stage2.o; }

stage3.nasm: stage2 stage2.c
	./stage2 >$@ <stage2.c

test: stage2
	test/exercise.sh ./$< >/dev/null
	test/compare.sh $(CC) ./stage2

coverage: stagecov
	test/exercise.sh ./$< >/dev/null 2>&1
	gcov $<
	grep -Ev 'internalerror|oom' $<.c.gcov | grep -C3 '#####'

.PHONY: coverage

stagecov.c: test/stagecov.sed 0prim.c 1stdlib.c 2lex.c 3safe.c 4parse.c
	sed -f test/stagecov.sed >$@ 0prim.c 1stdlib.c 2lex.c 3safe.c 4parse.c

stagecov: stagecov.c
	$(CC) -std=c99 -fprofile-arcs -ftest-coverage -o $@ $<

fmt:
	git diff -U0 --dst-prefix= HEAD~ -- . ':(exclude)test/badlex/*.c' | fill-changed-comments
	clang-format -i -style=google *.c test/*.c test/badparse/*.c
	pretty-mk Makefile
	set-shebang /usr/bin/env python3 -- test/*.py tool/*.py
	yapf -i test/*.py tool/*.py
	set-shebang /bin/sh -- test/*.sh tool/*.sh
	pretty-sh test/*.sed test/*.sh tool/*.sh
	rewrite-in-place tool/reformat.py -- grammar

.PHONY: fmt
