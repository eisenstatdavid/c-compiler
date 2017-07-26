#!/bin/sh
set -e
grep -o "^[[:space:]]*'[^']*'" tool/abbr.py | aspell list | sort | uniq -c
echo '#####'
sed -n 's/^[cinv][[:lower:]]* \([[:alpha:]_]\{1,\}\).*$/\1/p' ./*.c | sort -u >ident
grep -o '"[^"]*"' ./*.c | sed 's/\\[nrt]/ /g' | aspell list | sort -u | comm -23 - ident
rm -f ident
echo '#####'
! grep -E 'puts\("([^"\\]|\\.)"\)' ./*.c
! grep ' \\[nt]' ./*.c
! grep '\\[nt] ' ./*.c
sed -n "/^ \{2\}A/,/^}/p" 2lex.c >tmp && sort -u tmp | diff -u tmp - && rm -f tmp
{ grep 'keyword("' 2lex.c >tmp || echo No match; } && sort -u tmp | diff -u tmp - && rm -f tmp
{ grep '^ \{2\}EXPR' 4parse.c >tmp || echo No match; } && sort -u tmp | diff -u tmp - && rm -f tmp
{ grep '^ \{2\}TYPE' 4parse.c >tmp || echo No match; } && sort -u tmp | diff -u tmp - && rm -f tmp
sed -n "/^wordlets = /,/^}/s/'/ /gp" tool/abbr.py >tmp && sort -u tmp | diff -u tmp - && rm -f tmp
sed -n "/^words = /,/^}/s/'/ /gp" tool/abbr.py >tmp && sort -u tmp | diff -u tmp - && rm -f tmp
tool/abbr.py [0-9]*.c grammar
echo '#####'
tool/strangeconst.py [0-9]*.c
