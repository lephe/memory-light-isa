import collections
import re

Token = collections.namedtuple('Token', ['typ', 'value', 'line', 'column'])

def tokenize(code):
    """ Code adapté depuis https://docs.python.org/3/library/re.html"""

    token_specification = [

        # Operators
        ('operat',  r'add|sub|cmp|let|shift|readze|readse|jump|or|and|write|call|setctr|getctr|push|return|xor|asr'),

        # Values
        ('regist',  r'(?:r|R)[0-9]+'),
        ('direct', r'left|right'),
        ('number',    r'[+-]?(?:0x[0-9A-Fa-f]+|[0-9]+)'),
        ('commen',   r';.*'),
        ('condit', r'eq|z|neq|nz|sgt|slt|gt|ge|nc|lt|c|le'),
        ('counte',   r'pc|sp|a0|a1'),
        
        # Tokenizer stuff
        ('newlin',   r'\n'),
        ('skip',      r'[ \t]'),
        ('mismatch',  r'.')
    ]

    tok_regex = '|'.join('(?P<%s>%s)' % pair for pair in token_specification)

    line_num = 1
    line_start = 0
    for mo in re.finditer(tok_regex, code):
        kind = mo.lastgroup
        value = mo.group(kind)
        if kind == 'newlin':
            column = mo.start() - line_start
            line_start = mo.end()
            line_num += 1
            yield Token(kind, value, line_num-1, column)
        elif kind == 'skip':
            pass
        elif kind == 'mismatch':
            raise RuntimeError(f'{value!r} unexpected on line {line_num}')
        else:
            column = mo.start() - line_start
            yield Token(kind, value, line_num, column)

statements = '''
    add r3 r5 r6
    add r1 343
    shift left r2 31
    let r1 R3 ; commentaire
'''

for token in tokenize(statements):
    print(token)
