#!/bin/sh
set -e
sed -n '/^wordlets = {$/,/^}$/s/^[[:space:]]\{1,\}//p' tool/abbr.py | while read -r wordlet; do
sed "/${wordlet}/d" tool/abbr.py >abbr1.py
cat [0-9]*.c grammar | python3 abbr1.py | grep -q '^\[]$' && echo delete "${wordlet}"
{ echo "${wordlet}" | tr '[:lower:]' '[:upper:]'; echo "$(echo "${wordlet}" | sed 's/\(..\).*/\1/' | tr '[:lower:]' '[:upper:]')$(echo "${wordlet}" | sed 's/..//')"; echo "${wordlet}"; } >tmp
sed '/^words = {$/r tmp' abbr1.py >abbr2.py
cat [0-9]*.c grammar | python3 abbr2.py | grep -q '^\[]$' && echo modify "${wordlet}"
done
sed -n '/^words = {$/,/^}$/s/^[[:space:]]\{1,\}//p' tool/abbr.py | while read -r word; do
sed "/${word}/d" tool/abbr.py >abbr1.py
cat [0-9]*.c grammar | python3 abbr1.py | grep -q '^\[]$' && echo delete "${word}"
done
