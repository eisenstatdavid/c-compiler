#!/usr/bin/env python3
import operator
import subprocess
import sys

numbers = {
    sign * 2**exp + err
    for sign in (-1, 1) for exp in (0, 15, 16, 31) for err in (-1, 0, 1)
}
numbers = sorted(n for n in numbers if -2**31 <= n < 2**31)


def fmt(n):
    if n is None: return ''
    if n == -2**31: return '(%s - %s)' % (n + 1, 1)
    return str(n)


for x in [None] + numbers:
    for opc, opf in {
            '+': operator.add,
            '/': operator.floordiv,
            '%': operator.mod,
            '*': operator.mul,
            '-': operator.sub
    }.items():
        if x is None and opc != '-': continue
        for y in numbers:
            p = subprocess.Popen(
                [sys.argv[1]],
                stdin=subprocess.PIPE,
                stdout=subprocess.PIPE,
                stderr=subprocess.PIPE)
            prog = 'enum { Z = %s %s %s };\nnoreturn void start() {}\n' % (
                fmt(x), opc, fmt(y))
            _, err = p.communicate(prog.encode())
            try:
                z = opf(x or 0, y)
                if not (-2**31 <= z < 2**31): z = None
            except ZeroDivisionError:
                z = None
            if (not err) != (z is not None): print(prog)
