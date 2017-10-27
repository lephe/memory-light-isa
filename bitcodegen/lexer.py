""" lexer.py """
import re

from .enums import Token, LexType, inverse_possible_transition
from .errors import TokenError
from .util import Stack, huffman, sub


class Lexer(object):
    """ Code adapté depuis https://docs.python.org/3/library/re.html

    Prend une chaine de caractères encodées dans un langage pre-assembleur et
    génére des tokens en fonctions des operandes, et des valeures"""

    def __init__(self):
        token_specification = { \
            # Operators
            LexType.OPERATION : \
r'add|sub|cmp|let|shift|readze|readse|jump|or|and|write|call|setctr|getctr|push|return|xor|asr',

            # Values
            LexType.REGISTER  : r'(?:r|R)[0-9]+',
            LexType.DIRECTION : r'left|right',
            LexType.NUMBER    : r'[+-]?(?:0x[0-9A-Fa-f]+|[0-9]+)',
            LexType.COMMENT   : r';.*',
            LexType.CONDITION : r'eq|z|neq|nz|sgt|slt|gt|ge|nc|lt|c|le',
            LexType.COUNTER   : r'pc|sp|a0|a1',
            LexType.LABEL     : r'[a-zA-Z][a-zA-Z0-9]*',

            # Tokenizer stuff
            LexType.NEWLINE   : r'\n',
            LexType.SKIP      : r'[ \t]+',
            LexType.MISMATCH  : r'.',
            LexType.ENDFILE   : r'$',
        }

        tok_regex = '|'.join('(?P<%s>%s)' % (str(name).split(".")[1], re) \
        	for name, re in token_specification.items())

        self.rexp = re.compile(tok_regex)

        self.aliases = {\
            LexType.CONDITION : {"z":"eq", "nz":"neq", "nc":"ge", "c":"lt"}\
        }


    def lex(self, code):

        # Temporaire : 
        code = sub(code, inverse_possible_transition)

        line_num = 1
        line_start = 0

        for match in self.rexp.finditer(code):
            kindname = match.lastgroup
            value = match.group(kindname)
            kind = LexType.__members__[kindname]

            value = self.lex_alias(kind, value)
            value = self.lex_value(kindname, value)

            # Tokenization
            if kind is LexType.NEWLINE or kind is LexType.ENDFILE:
                column = match.start() - line_start
                line_start = match.end()
                line_num += 1
                yield Token(LexType.NEWLINE, None, line_num-1, column)

            elif kind is LexType.SKIP:
                pass 

            elif kind is LexType.MISMATCH:
                raise TokenError(f'invalid syntax at line {line_num} : {value}')

            else:
                column = match.start() - line_start
                yield Token(kind, value, line_num, column)

            if kind is LexType.ENDFILE:
                yield Token(LexType.ENDFILE, None, line_num, 0)


    def lex_alias(self, kind, value):
        if kind in self.aliases:
            if value in self.aliases[kindname]:
                return self.aliases[kindname][value]

        return value

    def lex_value(self, kindname, value):

        # Technique inspiré de http.server.BasicHTTPHandler

        method_name = f"lex_value_{kindname}"
        if hasattr(self, method_name):
            method = getattr(self, method_name)
            return method(value)

        else:
            return value


    def lex_value_NUMBER(self, value):
        """ value is a string in the form r'[+-]?(?:0x[0-9A-Fa-f]+|[0-9]+)' """

        if value[:2].lower() == "0x":
            return int(value, 16)
        else:
            return int(value)

    def lex_value_REGISTER(self, value):
        """ value is a string in the form r'(?:r|R)[0-9]+' """

        return int(value[1:])


if __name__ == '__main__':
    l = bitcodegen.Lexer()

    statements = '''\
        add r3 r5 r6
        add r1 343 ; Ceci est un commentaire
        sub r1 r2 -13
        shift left r3 -5
    '''

    for token in l.lex(statements):
        print(token)
