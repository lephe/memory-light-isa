import collections
import re
import heapq
import enum
import itertools


Token = collections.namedtuple('Token', ['typ', 'value', 'line', 'column'])


class TokenError(RuntimeError):
    pass


class Queue(collections.deque):
    def push(self, x):
        self.append_left()

    def is_empty(self):
        return len(self) == 0

TokenType = enum.Enum('TokenType', \
    "OPERATION REGISTER DIRECTION NUMBER COMMENT \
     CONDITION COUNTER NEWLINE SKIP MISMATCH ENDFILE")

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

        kind = globals()[kind]

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
    def __init__(self, tokens_generator = None):
        self.tokens_generator = tokens_generator
        self.queue = Queue()

    def parse(self, tokens_generator):
        self.tokens_generator = tokens_generator

        return self.run()

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
                except TokenError:
                    print(f"error at {token.line, token.column}")
                    raise

            elif token.typ is NEWLINE:
                pass

            elif token.typ is COMMENT:
                pass
            else:
                raise TokenError()

            while not self.queue.is_empty:
                yield self.queue.pop()


    # parseur d'opérateurs
    def asm_add(self, token):
        try :
            x = next(self.tokens_generator)
            y = next(self.tokens_generator)
            z = next(self.tokens_generator)

        except StopIteration:
            raise TokenError("not enought arguments for operand")

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
            raise TokenError("Invalid token")

    def asm_sub(self, token):
        try :
            x = next(self.tokens_generator)
            y = next(self.tokens_generator)
            z = next(self.tokens_generator)

        except StopIteration:
            raise TokenError("not enought arguments for operand")

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
            raise TokenError("Invalid token")

    def asm_cmp(self, token):
        raise NotImplementedError()

    def asm_let(self, token):
        raise NotImplementedError()

    def asm_shift(self, token):
        raise NotImplementedError()

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
    c = collections.COUNTER()

    c.update(token.value for token in gene if token is OPERATION)

    return c


def huffman(c):

    """take a collections.COUNTER, and generate an huffman-tree. O(n) = n log(n)
    worst case."""

    # Heap est une foret d'arbres étiquetées par la somme des frequences des
    # feuilles des arbres, où les feuilles des arbres sont étiquetés par les
    # clés de c. Heap est un tas-min pour des raisons de performance.

    heap = []
    for key, freq in c.most_common():

        # Initialisation : On fabrique une foret de feuilles. On ne garde en
        # mémoire que les feuilles et le chemin (en binaire) pour acceder à la
        # feuille.
        
        heap.append((freq, [("", key)]))

    if len(heap) == 0:
        return []

    if len(heap) == 1:
        # Unique key.
        return ["0", heap[0][0][1]]

    heapq.heapify(heap)

    while len(heap) > 1:

        # Algorithme glouton : on prend les arbres les moins fréquents, et on
        # les mets à coté.

        freq_x, x = heapq.heappop(heap)
        freq_y, y = heapq.heappop(heap)

        freq_z = freq_x + freq_y
        z = [("0" + pos, key) for pos, key in x] + [("1" + pos, key) for pos, key in y]
        
        # Et on repose le résultat dans le tas.        
        heapq.heappush(heap, (freq_z, z))

    # len(heap) = 1 ici.
    tot, tree = heap[0]

    return tree

statements = '''
    add r3 r5 r6
    add r1 343
    shift left r2 31
    let r1 R3 ; commentaire
'''

for token in lexer(statements):
    print(token)



def compile_asm(s):

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






