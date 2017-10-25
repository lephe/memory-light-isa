import collections
import re
import heapq
import enum
import itertools
from .utils import huffman, Queue, Stack
from .errors import TokenError, ParserError

Token = collections.namedtuple('Token', ['typ', 'value', 'line', 'column'])

TokenType = enum.Enum('TokenType', \
    "OPERATION REGISTER DIRECTION NUMBER COMMENT CONDITION COUNTER NEWLINE SKIP MISMATCH ENDFILE LABEL")

# Petit hack pour ajouter les valeurs de mon énum dans scope global
for name, member in TokenType.__members__.items():
    globals()[name] = member

def lexer(code):
    """ Code adapté depuis https://docs.python.org/3/library/re.html
    
    Prend une chaine de caractères encodées dans un langage pre-assembleur et
    génére des tokens en fonctions des operandes, et des valeures"""

    token_specification = [
        # Operators
        (OPERATION, r'add|sub|cmp|let|shift|readze|readse|jump|or|and|write|call|setctr|getctr|push|return|xor|asr'),

        # Values
        (REGISTER,  r'(?:r|R)[0-9]+'),
        (DIRECTION, r'left|right'),
        (NUMBER,    r'[+-]?(?:0x[0-9A-Fa-f]+|[0-9]+)'),
        (COMMENT,   r';.*'),
        (CONDITION, r'eq|z|neq|nz|sgt|slt|gt|ge|nc|lt|c|le'),
        (COUNTER,   r'pc|sp|a0|a1'),
        (LABEL,     r'[a-zA-Z][a-zA-Z0-9]*:')
        
        # Tokenizer stuff
        (NEWLINE,   r'\n'),
        (SKIP,      r'[ \t]+'),
        (MISMATCH,  r'.'),
        (ENDFILE,   r'$')
    ]

    tok_regex = '|'.join('(?P<%s>%s)' % (str(name).split(".")[1], re) for name, re in token_specification)

    line_num = 1
    line_start = 0
    for mo in re.finditer(tok_regex, code):
        kind = mo.lastgroup
        value = mo.group(kind)

        kind = TokenType.__members__[kind]

        if kind is NEWLINE:
            column = mo.start() - line_start
            line_start = mo.end()
            line_num += 1
            yield Token(kind, value, line_num-1, column)

        elif kind is SKIP:
            pass

        elif kind is MISMATCH:
            raise TokenError(f'{value!r} unexpected on line {line_num}')

        elif kind is ENDFILE:
            # Ajout d'une fin de ligne à la fin de chaque programme.
            yield Token(NEWLINE, value, line_num, column)
            yield Token(ENDFILE, value, line_num, column)

        else:
            column = mo.start() - line_start
            yield Token(kind, value, line_num, column)



class Parser(object):
    def __init__(self, lexer = None):
        self.lexer = lexer
        self.stack = Stack()

    def parse(self, lexer):
        self.lexer = lexer

        return self.run()

    def unstack_until(self, predicate, * , want_res=False):

        if want_res:

            res = Stack()
            while predicate(self.stack[-1]):
                res.push(self.stack.pop())

            return tuple(res)

        else:
            while predicate(self.stack[-1]):
                self.stack.pop()

            return ()  # Return an empty tuple.

    def run(self):
        for token in tokens_generator:
            if token.typ is OPERATION:
                # code inspiré par le module python http.server, classe BaseHTTPRequestHandler
                mnname = 'asm_' + token.value
                if not hasattr(self, mnname):
                    raise NotImplementedError()

                method = getattr(self, mnname)
                try:
                    method(token)
                except ParserError:
                    print(f"error at {token.line, token.column}")
                    raise

            elif token.typ is NEWLINE:
                pass

            elif token.typ is COMMENT:
                pass
            else:
                raise ParserError()

            while not self.queue.is_empty:
                yield self.queue.pop()


    # parseur d'opérateurs
    def asm_add(self, token):
        try :
            x = next(self.tokens_generator)
            y = next(self.tokens_generator)
            z = next(self.tokens_generator)

        except StopIteration:
            raise ParserError("not enought arguments for operand")

        if (x, y, z) is (REGISTER, REGISTER, REGISTER):
            self.queue.push(Token(token.typ, "add3", token.line, token.column))
            self.queue.push(x)
            self.queue.push(y)
            self.queue.push(z)

        elif (x, y, z) is (REGISTER, REGISTER, NUMBER):
            self.queue.push(Token(token.typ, "add3i", token.line, token.column))
            self.queue.push(x)
            self.queue.push(y)
            self.queue.push(z)

        elif (x, y) is (REGISTER, REGISTER):
            self.queue.push(Token(token.typ, "add2", token.line, token.column))
            self.queue.push(x)
            self.queue.push(y)
        
        elif (x, y) is (REGISTER, NUMBER):
            self.queue.push(Token(token.typ, "add2i", token.line, token.column))
            self.queue.push(x)
            self.queue.push(y)
        else:
            raise ParserError("Invalid token")

    def asm_sub(self, token):
        try :
            x = next(self.tokens_generator)
            y = next(self.tokens_generator)
            z = next(self.tokens_generator)

        except StopIteration:
            raise ParserError("not enought arguments for operand")

        if (x, y, z) is (REGISTER, REGISTER, REGISTER):
            self.queue.push(Token(token.typ, "sub3", token.line, token.column))
            self.queue.push(x)
            self.queue.push(y)
            self.queue.push(z)

        elif (x, y, z) is (REGISTER, REGISTER, NUMBER):
            self.queue.push(Token(token.typ, "sub3i", token.line, token.column))
            self.queue.push(x)
            self.queue.push(y)
            self.queue.push(z)

        elif (x, y) is (REGISTER, REGISTER):
            self.queue.push(Token(token.typ, "sub2", token.line, token.column))
            self.queue.push(x)
            self.queue.push(y)
        
        elif (x, y) is (REGISTER, NUMBER):
            self.queue.push(Token(token.typ, "sub2i", token.line, token.column))
            self.queue.push(x)
            self.queue.push(y)
        else:
            raise ParserError("Invalid token")

    def asm_cmp(self, token):
        try : 
            x = next(self.tokens_generator)
            y = next(self.tokens_generator)

        except StopIteration:
            raise ParserError("not enought arguments for operand")

        if (x, y) is (REGISTER, NUMBER):
            self.queue.push(Token(token.typ, "cmpi", token.line, token.column))
            self.queue.push(x)
            self.queue.push(y)

        elif (x, y) is (REGISTER, REGISTER):
            self.queue.push(Token(token.typ, "cmp", token.line, token.column))
            self.queue.push(x)
            self.queue.push(y)

        else:
            raise ParserError("Invalid token")

    def asm_let(self, token):
        try : 
            x = next(self.tokens_generator)
            y = next(self.tokens_generator)

        except StopIteration:
            raise ParserError("not enought arguments for operand")

        if (x, y) is (REGISTER, NUMBER):
            self.queue.push(Token(token.typ, "leti", token.line, token.column))
            self.queue.push(x)
            self.queue.push(y)

        elif (x, y) is (REGISTER, REGISTER):
            self.queue.push(Token(token.typ, "let", token.line, token.column))
            self.queue.push(x)
            self.queue.push(y)

        else:
            raise ParserError("Invalid token")

    def asm_shift(self, token):
        try:
            x = next(self.tokens_generator)
            y = next(self.tokens_generator)
            z = next(self.tokens_generator)

        except StopIteration:
            raise ParserError("not enought arguments for operand")

        if (x, y, z) is (DIRECTION, REGISTER, NUMBER):
            self.queue.push(Token(token.typ, "shift", token.line, token.column))
            self.queue.push(x)
            self.queue.push(y)
            self.queue.push(z)

        else:
            raise ParserError("Invalid token")

    def asm_readze(self, token):
        raise NotImplementedError()

    def asm_readse(self, token):
        raise NotImplementedError()

    def asm_jump(self, token):
        raise NotImplementedError()

    def asm_or(self, token):
        raise NotImplementedError()

    def asm_and(self, token):
        raise NotImplementedError()

    def asm_write(self, token):
        raise NotImplementedError()

    def asm_call(self, token):
        raise NotImplementedError()

    def asm_setctr(self, token):
        raise NotImplementedError()

    def asm_getctr(self, token):
        raise NotImplementedError()

    def asm_push(self, token):
        raise NotImplementedError()

    def asm_return(self, token):
        raise NotImplementedError()

    def asm_xor(self, token):
        raise NotImplementedError()

    def asm_asr(self, token):
        raise NotImplementedError()


def count_oprations(gene):
    c = collections.Counter()

    c.update(token.value for token in gene if token is OPERATION)

    return c

statements = '''
    add r3 r5 r6
    add r1 343
'''

for token in lexer(statements):
    print(token)



def compile_asm(s, * ,generate_tree=False):

    # tokenize the pre-asm
    lex = lexer(s)

    # parse to convert in asm
    par = Parser().parse(lex)

    # duplicate the iterator
    par1, par2 = itertools.tee(par)

    # Generate and save the huffman-tree of the opcodes
    c = count_oprations(par1)
    hufftree = huffman(c)

    with open("opcode.txt", "w+") as f:
        for opcode, memonic in hufftree:
            f.write(f"{memonic} {opcode}\n")

    jumpcode = None




compile_asm(statements)

