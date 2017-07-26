#!/usr/bin/env python3
import fileinput
import re
last = None
for line in fileinput.input():
    line = re.sub(r'[.Nt]', '', line)
    line = re.sub(r'[0-9a-f]{2}', lambda c: chr(int(c.group(0), 16)), line)
    m = re.match(r'(<[^>]*>) -> (.*)$', line)
    if m.group(1) != last:
        print(m.group(1), '::=')
        last = m.group(1)
    print('\t|', m.group(2))
