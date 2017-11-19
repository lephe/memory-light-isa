#!/usr/bin/env python3
import re

from .enums import Token, LexType
from .errors import TokenError
from .util import Stack, huffman, sub


class Lexer(object):
    """ Code adapté depuis https://docs.python.org/3/library/re.html

    Prend une chaine de caractères encodées dans un langage pre-assembleur et
    génére des tokens en fonctions des operandes, et des valeures"""

    def __init__(self, possible_transitions):
        token_specification = {


            # Operators
            LexType.OPERATION: r'\b(?:add|sub|cmp|let|shift|readze|readse|' + \
                               r'jump|or|and|write|call|setctr|getctr|push|' +\
                               r'return|xor|asr|pop)\b',

            # Values
            LexType.REGISTER:   r'\b(?:r|R)[0-9]+\b',
            LexType.DIRECTION:  r'\b(?:left|right)\b',
            LexType.NUMBER:     r'[+-]?\b(?:0x[0-9A-Fa-f]+|[0-9]+)\b',
            LexType.COMMENT:    r';.*',
            LexType.CONDITION:  r'\b(?:eq|z|neq|nz|sgt|slt|gt|ge|nc|lt|c|v|le)\b',
            LexType.MEMCOUNTER: r'\b(?:pc|sp|a0|a1)\b',

            # LABELS/IMPORTS
            LexType.LABEL:      r'\b[a-zA-Z_][a-z_A-Z0-9]*:?',
            LexType.INCLUDE:    r'\.include [a-zA-z_][a-z_A-Z0-9]*\b',

            # Tokenizer stuff
            LexType.NEWLINE:  r'\n',
            LexType.SKIP:     r'[ \t]+',
            LexType.MISMATCH: r'.+',
            LexType.ENDFILE:  r'$',
        }

        self.possible_transitions = possible_transitions

        tok_regex = '|'.join('(?P<%s>%s)' % (str(name).split(".")[1], re)
                             for name, re in token_specification.items())

        self.rexp = re.compile(tok_regex)

        self.aliases = {
            LexType.CONDITION: {"z": "eq", "nz": "neq", "nc": "ge", "c": "lt",
                                "le": "v"}
        }

        self.includes = set()

    def lex(self, code, name="", directory="."):

        if name in self.includes:
            return

        self.includes.add(name)

        # Temporaire :
        # code = sub(code, inverse_possible_transition)

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
                raise TokenError(
                    f'invalid syntax at line {line_num} : {value}')

            elif kind is LexType.LABEL:
                column = match.start() - line_start
                # yield Token(LexType.OPERATION, "label", line_num, column)
                yield Token(LexType.LABEL, value, line_num, column)

            elif kind is LexType.INCLUDE:
                value = value[9:]
                filename = f"{directory}/{value}.s"
                with open(filename, "r") as f:
                    s = f.read()

                    for new, olds in self.possible_transitions.items():
                        olds.sort(reverse=True, key=lambda x: len(x))
                        s = re.sub("(" + "|".join(olds) + ")", new, s)

                    for token in self.lex(s, name=value, directory=directory):
                        yield token

            else:
                column = match.start() - line_start
                yield Token(kind, value, line_num, column)

            if kind is LexType.ENDFILE:
                yield Token(LexType.ENDFILE, None, line_num, 0)

    def lex_alias(self, kind, value):
        if kind in self.aliases:
            if value in self.aliases[kind]:
                return self.aliases[kind][value]

        return value

    def lex_value(self, kindname, value):

        # Technique inspiré de http.server.BasicHTTPHandler

        method_name = f"lex_value_{kindname}"
        if hasattr(self, method_name):
            method = getattr(self, method_name)
            return method(value)

        return value

    def lex_value_NUMBER(self, value):
        """ value is a string in the form r'[+-]?(?:0x[0-9A-Fa-f]+|[0-9]+)' """

        if "0x" in value.lower():
            return int(value, 16)
        if value[:3].lower() == "-0x":
            return -int(value[1:], 16)

        return int(value)

    def lex_value_REGISTER(self, value):
        """ value is a string in the form r'(?:r|R)[0-9]+' """

        return int(value[1:])

    def lex_value_LABEL(self, value):

        if ":" in value:
            return value[:-1]
        else:
            return value
