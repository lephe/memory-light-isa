import collections
import itertools

from .enums import ValueType, LexType
from .lexer import Lexer
from .parser import Parser
from .util import huffman


# Language specification

possible_transition = {
    "add":    ["add2", "add2i", "add3", "add3i"],
    "and":    ["and2", "and2i", "and3", "and3i"],
    "sub":    ["sub2", "sub2i", "sub3", "sub3i"],
    "or":     ["or2", "or2i", "or3", "or3i"],
    "xor":    ["xor3", "xor3i"],
    "cmp":    ["cmp", "cmpi"],
    "let":    ["let", "leti"],
    "shift":  ["shift"],
    "readze": ["readze"],
    "readse": ["readse"],
    "jump":   ["jump", "jumpif"],
    "write":  ["write"],
    "call":   ["call"],
    "setctr": ["setctr"],
    "getctr": ["getctr"],
    "push":   ["push"],
    "return": ["return"],
    "asr":    ["asr3"],
    "pop":    ["pop"]}


VT = ValueType

type_specs = {
    LexType.NUMBER:  [VT.UCONSTANT, VT.SCONSTANT, VT.RADDRESS,
                      VT.AADDRESS, VT.SHIFTVAL, VT.SIZE],
    LexType.DIRECTION:  [VT.DIRECTION],
    LexType.CONDITION:  [VT.CONDITION],
    LexType.MEMCOUNTER: [VT.MEMCOUNTER],
    LexType.REGISTER:   [VT.REGISTER]
}

asr_specs = {
    "add2":    (VT.REGISTER, VT.REGISTER),
    "add2i":   (VT.REGISTER, VT.UCONSTANT),
    "add3":    (VT.REGISTER, VT.REGISTER, VT.REGISTER),
    "add3i":   (VT.REGISTER, VT.REGISTER, VT.UCONSTANT),

    "sub2":    (VT.REGISTER, VT.REGISTER),
    "sub2i":   (VT.REGISTER, VT.UCONSTANT),
    "sub3":    (VT.REGISTER, VT.REGISTER, VT.REGISTER),
    "sub3i":   (VT.REGISTER, VT.REGISTER, VT.UCONSTANT),

    "cmp":     (VT.REGISTER, VT.REGISTER),
    "cmpi":    (VT.REGISTER, VT.SCONSTANT),

    "let":     (VT.REGISTER, VT.REGISTER),
    "leti":    (VT.REGISTER, VT.SCONSTANT),

    "shift":   (VT.DIRECTION, VT.REGISTER, VT.SHIFTVAL),

    "readze":  (VT.MEMCOUNTER, VT.SIZE, VT.REGISTER),

    "readse":  (VT.MEMCOUNTER, VT.SIZE, VT.REGISTER),

    "jump":    (VT.RADDRESS,),
    "jumpif":  (VT.CONDITION, VT.RADDRESS),

    "or2":     (VT.REGISTER, VT.REGISTER),
    "or2i":    (VT.REGISTER, VT.SCONSTANT),
    "or3":     (VT.REGISTER, VT.REGISTER, VT.REGISTER),
    "or3i":    (VT.REGISTER, VT.REGISTER, VT.SCONSTANT),

    "and2":    (VT.REGISTER, VT.REGISTER),
    "and2i":   (VT.REGISTER, VT.SCONSTANT),
    "and3":    (VT.REGISTER, VT.REGISTER, VT.REGISTER),
    "and3i":   (VT.REGISTER, VT.REGISTER, VT.SCONSTANT),

    "write":   (VT.MEMCOUNTER, VT.SIZE, VT.REGISTER),
    "call":    (VT.AADDRESS,),
    "setctr":  (VT.MEMCOUNTER, VT.REGISTER),
    "getctr":  (VT.MEMCOUNTER, VT.REGISTER),
    "push":    (VT.SIZE, VT.REGISTER),
    "pop":     (VT.SIZE, VT.REGISTER),
    "return":  (),

    "xor3":    (VT.REGISTER, VT.REGISTER, VT.REGISTER),
    "xor3i":   (VT.REGISTER, VT.REGISTER, VT.SCONSTANT),

    "asr3":    (VT.REGISTER, VT.REGISTER, VT.SHIFTVAL),

    "reserved1":   (),
    "reserved2":   (),
    "reserved3":   ()}

del VT


def count_operations(it):
    c = collections.Counter()
    for line in it:
        c.update([line.funcname])

    return c


def compile_asm(s, *, generate_tree=False):

    # tokenize the pre-asm
    lexer = Lexer()
    gen_lex = lexer.lex(s)

    # parse to convert in asm
    parser = Parser(gen_lex, possible_transition, asr_specs, type_specs)

    # duplicate the iterator
    par1, par2 = itertools.tee(parser.run())

    # Generate and save the huffman-tree of the opcodes
    c = count_operations(par1)
    hufftree = {b: a for a, b in huffman(c)}

    with open("opcode.txt", "w+") as f:
        for opcode, memonic in hufftree.items():
            f.write(f"{memonic} {opcode}\n")

    import pprint
    for x in par2:
        pprint.pprint(x)
