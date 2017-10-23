import collections
import re
import heapq
import enum


Token = collections.namedtuple('Token', ['typ', 'value', 'line', 'column'])


class Queue(collections.deque):
    def push(self, x):
        self.append_left()

    def is_empty(self):
        return len(self) == 0

TokenType = enum.Enum('TokenType', \
    "operation register direction number comment \
     condition counter newline skip mismatch endfile")

def lexer(code):
    """ Code adapté depuis https://docs.python.org/3/library/re.html
    
    Prend une chaine de caractères encodées dans un langage pre-assembleur et
    génére des tokens en fonctions des operandes, et des valeures"""

    token_specification = [

        # Operators
        (TokenType.operation, r'add|sub|cmp|let|shift|readze|readse|jump|or|and|write|call|setctr|getctr|push|return|xor|asr'),

        # Values
        (TokenType.register,  r'(?:r|R)[0-9]+'),
        (TokenType.direction, r'left|right'),
        (TokenType.number,    r'[+-]?(?:0x[0-9A-Fa-f]+|[0-9]+)'),
        (TokenType.comment,   r';.*'),
        (TokenType.condition, r'eq|z|neq|nz|sgt|slt|gt|ge|nc|lt|c|le'),
        (TokenType.counter,   r'pc|sp|a0|a1'),
        
        # Tokenizer stuff
        (TokenType.newline,   r'\n'),
        (TokenType.skip,      r'[ \t]'),
        (TokenType.mismatch,  r'.'),
        (TokenType.endfile,   r'$')
    ]

    tok_regex = '|'.join('(?P<%s>%s)' % (str(name).split(".")[1], re) for name, re in token_specification)

    line_num = 1
    line_start = 0
    for mo in re.finditer(tok_regex, code):
        kind = mo.lastgroup
        value = mo.group(kind)
        if kind is TokenType.newline:
            column = mo.start() - line_start
            line_start = mo.end()
            line_num += 1
            yield Token(kind, value, line_num-1, column)

        elif kind is TokenType.skip:
            pass

        elif kind is TokenType.mismatch:
            raise RuntimeError(f'{value!r} unexpected on line {line_num}')

        elif kind is TokenType.endfile:
            # Ajout d'une fin de ligne à la fin de chaque programme.
            yield Token(TokenType.newline, value, line_num, column)
            yield Token(TokenType.endfile, value, line_num, column)

        else:
            column = mo.start() - line_start
            yield Token(kind, value, line_num, column)



class Parser(object):
    def __init__(self, tokens_generator):
        self.tokens_generator = tokens_generator
        self.queue = Queue()

    def parse(self, tokens_generator):
        self.tokens_generator = tokens_generator
        self.run()

    def run(self):
        for token in tokens_generator:
            if token.typ is TokenType.operation:
                # code inspiré par le module python http.server, classe BaseHTTPRequestHandler
                mnname = 'asm_' + token.value
                if not hasattr(self, mnname):
                    raise NotImplementedError()

                method = getattr(self, mnname)
                method(token)

            elif token.typ is TokenType.newline:
                pass

            elif token.typ is TokenType.comment:
                pass

            while not self.queue.is_empty:
                yield self.queue.pop()

    def asm_add(self, token):
        raise NotImplementedError()

    def asm_sub(self, token):
        raise NotImplementedError()

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

def huffman(c):

    """take a collections.ounter, and generate an huffman-tree. O(n) = n log(n)
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

        z = [("0" + pos, key) for pos, key in x] + [("1" + pos, key) for pos, key in y]
        freq_z = freq_x + freq_y
        
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


print("\n\n\n")

c = collections.Counter()

c.update(("a" for i in range(10)))
c.update(("b" for i in range(10)))
c.update(("c" for i in range(10)))
c.update(("d" for i in range(10)))
c.update(("e" for i in range(10)))
c.update(("f" for i in range(10)))
c.update(("g" for i in range(10)))
c.update(("h" for i in range(10)))

print(c)

print(huffman(c))
