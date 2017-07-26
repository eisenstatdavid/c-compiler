#!/bin/sh
set -e
! grep "$(printf '.\t')" ./*.nasm
! grep "$(printf '^\t.*:$')" ./*.nasm
! grep "$(printf '^[^\t].*[^:]$')" ./*.nasm
! grep '[[:space:]]\{2\}' ./*.nasm
! grep '[[:space:]]\*' ./*.nasm
! grep '\*[[:space:]]' ./*.nasm
! grep '[^ ]+' ./*.nasm
! grep '+[^ ]' ./*.nasm
! grep '[[:space:]],' ./*.nasm
! grep ',[^ ]' ./*.nasm
! grep '[^ ]-' ./*.nasm
! grep -E -- '-([^ 0-9]|[0-9]+[^0-9])' ./*.nasm
! grep '[^ ]\.' ./*.nasm
! grep '\.[[:space:]]' ./*.nasm
! grep '[[:space:]]:' ./*.nasm
! grep ':.' ./*.nasm
! grep '[[:space:]]=' ./*.nasm
! grep '=[[:space:]]' ./*.nasm
! grep '\[[[:space:]]' ./*.nasm
! grep '[[:space:]]\]' ./*.nasm
