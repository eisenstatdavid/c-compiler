#!/usr/bin/env python3
import fileinput
import re
for line in fileinput.input():
    for s in re.findall('0x[0-9A-Fa-f]+|[0-9]+', line):
        n = int(s, 0)
        if (n & (n + 1)) and (n & (n - 1)):
            print(n)
