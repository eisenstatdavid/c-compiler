#!/usr/bin/env python3
def isnonterm(word):
    return word.startswith('<') and word.endswith('>')


def tosym(word):
    return 'tN' [isnonterm(word)] + ''.join('{:02x}'.format(ord(letter))
                                            for letter in word)


with open('grammar') as f:
    for line in f:
        words = line.split()
        if not words:
            pass
        elif words[0] == '|':
            print(ident, '->', *map(tosym, words[1:]), '.')
        else:
            assert (len(words) == 2) and isnonterm(words[0]) and (
                words[1] == '::='), line
            ident = tosym(words[0])
