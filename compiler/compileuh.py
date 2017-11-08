import collections
import itertools
import re

from .enums import ValueType, LexType
from .lexer import Lexer
from .parser import Parser
from .util import huffman
from .back_end import MemonicBackEnd


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
    "jump":   ["jump", "jumpif", "jumpl", "jumpifl"],
    "write":  ["write"],
    "call":   ["call", "calll"],
    "setctr": ["setctr"],
    "getctr": ["getctr"],
    "push":   ["push"],
    "return": ["return"],
    "asr":    ["asr3"],
    "pop":    ["pop"],
    "label":  ["label"]}


VT = ValueType

type_specs = {
    LexType.NUMBER:  [VT.UCONSTANT, VT.SCONSTANT, VT.RADDRESS,
                      VT.AADDRESS, VT.SHIFTVAL, VT.SIZE],
    LexType.DIRECTION:  [VT.DIRECTION],
    LexType.CONDITION:  [VT.CONDITION],
    LexType.MEMCOUNTER: [VT.MEMCOUNTER],
    LexType.REGISTER:   [VT.REGISTER],
    LexType.LABEL:      [VT.LABEL]
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
    "jumpl":   (VT.LABEL,),
    "jumpifl": (VT.CONDITION, VT.LABEL),

    "or2":     (VT.REGISTER, VT.REGISTER),
    "or2i":    (VT.REGISTER, VT.UCONSTANT),
    "or3":     (VT.REGISTER, VT.REGISTER, VT.REGISTER),
    "or3i":    (VT.REGISTER, VT.REGISTER, VT.UCONSTANT),

    "and2":    (VT.REGISTER, VT.REGISTER),
    "and2i":   (VT.REGISTER, VT.UCONSTANT),
    "and3":    (VT.REGISTER, VT.REGISTER, VT.REGISTER),
    "and3i":   (VT.REGISTER, VT.REGISTER, VT.UCONSTANT),

    "write":   (VT.MEMCOUNTER, VT.SIZE, VT.REGISTER),
    "call":    (VT.RADDRESS,),
    "calll":   (VT.LABEL,),
    "setctr":  (VT.MEMCOUNTER, VT.REGISTER),
    "getctr":  (VT.MEMCOUNTER, VT.REGISTER),
    "push":    (VT.SIZE, VT.REGISTER),
    "pop":     (VT.SIZE, VT.REGISTER),
    "return":  (),

    "xor3":    (VT.REGISTER, VT.REGISTER, VT.REGISTER),
    "xor3i":   (VT.REGISTER, VT.REGISTER, VT.UCONSTANT),

    "asr3":    (VT.REGISTER, VT.REGISTER, VT.SHIFTVAL),

    "reserved1":   (),
    "reserved2":   (),
    "reserved3":   (),

    "label":       (VT.LABEL,)}

del VT


default_opcode = {
    "add2":    "0000",
    "add2i":   "0001",
    "sub2":    "0010",
    "sub2i":   "0011",
    "cmp":     "0100",
    "cmpi":    "0101",
    "let":     "0110",
    "leti":    "0111",
    "shift":   "1000",
    "readze":  "10010",

    "pop":     "1001001",

    "readse":  "10011",
    "jump":    "1010",
    "jumpif":  "1011",
    "or2":     "110000",
    "or2i":    "110001",
    "and2":    "110010",
    "and2i":   "110011",
    "write":   "110100",
    "call":    "110101",
    "setctr":  "110110",
    "getctr":  "110111",
    "push":    "1110000",
    "return":  "1110001",
    "add3":    "1110010",
    "add3i":   "1110011",
    "sub3":    "1110100",
    "sub3i":   "1110101",
    "and3":    "1110110",
    "and3i":   "1110111",
    "or3":     "1111000",
    "or3i":    "1111001",
    "xor3":    "1111010",
    "xor3i":   "1111011",
    "asr3":    "1111100",
    "reserved1":   "1111101",
    "reserved2":   "1111110",
    "reserved3":   "1111111"}


def count_operations(c, it):
    for line in it:
        c.update([line.funcname])

    return c


def compile_asm(s, *, back_end=MemonicBackEnd, generate_tree=False):

    # Start by translating .p to .ps
    for new, olds in possible_transition.items():
        olds.sort(reverse=True, key=lambda x: len(x))
        s = re.sub("(" + "|".join(olds) + ")", new, s)

    # tokenize the pre-asm
    lexer = Lexer()
    gen_lex = lexer.lex(s)

    # parse to convert in asm
    parser = Parser(gen_lex, possible_transition, asr_specs, type_specs)

    if generate_tree:
        # duplicate the iterator
        par1, par = itertools.tee(parser.run())

        # Generate and save the huffman-tree of the opcodes
        c = collections.Counter()
        for key in default_opcode.keys():
            if key[:-1] != "reserved":
                c[key] = 0

        count_operations(c, par1)
        hufftree = {b: a for a, b in huffman(c)}

        with open("opcode.txt", "w+") as f:
            for opcode, memonic in hufftree.items():
                f.write(f"{memonic} {opcode}\n")
    else:
        par = parser.run()
        hufftree = default_opcode

    out = back_end(hufftree, par)

    return out
