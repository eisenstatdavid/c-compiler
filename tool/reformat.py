#!/usr/bin/env python3
import fileinput


def isnonterm(word):
    return word.startswith('<') and word.endswith('>')


dalts = {}
for line in fileinput.input():
    words = line.split()
    if not words:
        pass
    elif words[0] == '|':
        alt = words[1:]
        if not alt:
            assert ident.endswith('list>') or ident.endswith(
                'opt>') or ident.endswith('rest>')
        alts.append(alt)
    else:
        assert (len(words) == 2) and isnonterm(words[0]) and (
            words[1] == '::='), line
        ident = words[0]
        dalts[ident] = alts = []
for ident, alts in dalts.items():
    alts.sort()

sccs = []
s = []
onstack = set()
index = {}
lowlink = {}


def strongconnect(ident):
    lowlink[ident] = index[ident] = len(index)
    s.append(ident)
    onstack.add(ident)
    for alt in dalts[ident]:
        for word in alt:
            if isnonterm(word):
                assert word in dalts, word
                if word not in index:
                    strongconnect(word)
                    lowlink[ident] = min(lowlink[ident], lowlink[word])
                elif word in onstack:
                    lowlink[ident] = min(lowlink[ident], index[word])
    if lowlink[ident] == index[ident]:
        scc = []
        while True:
            word = s.pop()
            onstack.remove(word)
            scc.append(word)
            if word == ident:
                break
        scc.reverse()
        sccs.append(scc)


strongconnect('<decllist>')
sccs.sort(key=lambda scc: index[scc[0]])
order = sum(sccs, [])
assert set(order) == set(dalts), set(dalts) - set(order)
for ident in order:
    alts = dalts[ident]
    print(ident, '::=')
    for alt in alts:
        for word in alt:
            if not isnonterm(word):
                assert '<{}>'.format(word) not in dalts, word
        print('\t|', *alt)
