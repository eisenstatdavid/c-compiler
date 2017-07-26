#!/usr/bin/env python3
import fileinput
import re
import string
wordlets = {
    'add',
    'and',
    'arg',
    'arr',
    'asgn',
    'assert',
    'begin',
    'bin',
    'bit',
    'block',
    'bool',
    'brace',
    'bracket',
    'break',
    'buf',
    'byte',
    'c',
    'call',
    'char',
    'child',
    'comma',
    'cond',
    'const',
    'consume',
    'continue',
    'crit',
    'de',
    'decl',
    'decr',
    'def',
    'div',
    'else',
    'emit',
    'end',
    'enum',
    'eq',
    'err',
    'eval',
    'ex',
    'exit',
    'expr',
    'find',
    'func',
    'ge',
    'get',
    'global',
    'gt',
    'i',
    'ident',
    'if',
    'incr',
    'init',
    'insert',
    'int',
    'internal',
    'is',
    'jmp',
    'kind',
    'l',
    'label',
    'last',
    'le',
    'lex',
    'list',
    'lit',
    'local',
    'log',
    'lt',
    'match',
    'max',
    'memo',
    'mod',
    'mul',
    'n',
    'ne',
    'neg',
    'new',
    'next',
    'no',
    'node',
    'noreturn',
    'not',
    'op',
    'opt',
    'or',
    'out',
    'param',
    'paren',
    'parse',
    'peek',
    'pos',
    'post',
    'prev',
    'prim',
    'r',
    'read',
    'ref',
    'rel',
    'rest',
    'ret',
    'safe',
    'semi',
    'start',
    'state',
    'std',
    'stmt',
    'str',
    'sub',
    'subscr',
    'tok',
    'type',
    'un',
    'var',
    'void',
    'write',
}
wordsre = re.compile("(?:{})+$".format("|".join(wordlets)), re.I)
words = {
    'BUFSIZ',
    'EOF',
    'FAILURE',
    'FOR',
    'Line',
    'MIN',
    'MINUS',
    'PLUS',
    'RETURN',
    'SUCCESS',
    'UCHAR',
    'align',
    'eflush',
    'epilogue',
    'eputc',
    'eputd',
    'eputdln',
    'eputnegd',
    'eputs',
    'fildes',
    'flush',
    'for',
    'include',
    'isalnum',
    'isalpha',
    'isdigit',
    'isodigit',
    'isprint',
    'isspace',
    'keyword',
    'len',
    'main',
    'msg',
    'name',
    'offset',
    'oom',
    'panic',
    'prologue',
    'putc',
    'putd',
    'putdln',
    'putnegd',
    'puts',
    'reg',
    'res',
    'return',
    'section',
    'size',
    'status',
    'unistd',
    'val',
}
words.update(string.ascii_letters)
badidents = {
    ident
    for line in fileinput.input() for ident in re.findall(
        '[A-Za-z]+',
        re.sub('"(?:[^"\\\\]|\\\\.)*"|\'(?:[^\'\\\\]|\\\\.)\'|//.*', ' ',
               line)) if ident not in words and not wordsre.match(ident)
}
print(sorted(badidents))