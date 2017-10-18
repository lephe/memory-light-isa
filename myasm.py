#!/usr/bin/env python3

from collections import namedtuple


Command = namedtuple("Command", ["opcode", "operands"])
Condition = namedtuple("Condition", ["opcode"])
Counter = namedtuple("Counter", ["opcode"])

commands = {\
    "add2"   : Command(   "0000", ("reg", "reg")),
    "add2i"  : Command(   "0001", ("reg", "const")),
    "sub2"   : Command(   "0010", ("reg", "reg")),
    "sub2i"  : Command(   "0011", ("reg", "const")),
    "cmp"    : Command(   "0100", ("reg", "reg")),
    "cmpi"   : Command(   "0101", ("reg", "const")),
    "let"    : Command(   "0110", ("reg", "reg")),
    "leti"   : Command(   "0111", ("reg", "const")),
    "shift"  : Command(   "1000", ("dir", "reg", "shiftval")),
    "readze" : Command(  "10010", ("ctr", "size", "reg")),
    "readse" : Command(  "10011", ("ctr", "size", "reg")),
    "jump"   : Command(   "1010", ("addr",)),
    "jumpif" : Command(   "1011", ("cond", "addr")),
    "or2"    : Command( "110000", ("reg", "reg")),
    "or2i"   : Command( "110001", ("reg", "const")),
    "and2"   : Command( "110010", ("reg", "reg")),
    "and2i"  : Command( "110011", ("reg", "const")),
    "write"  : Command( "110100", ("ctr", "size", "reg")),
    "call"   : Command( "110101", ("addr",)),
    "setctr" : Command( "110110", ("ctr", "reg")),
    "getctr" : Command( "110111", ("ctr", "reg")),
    "push"   : Command("1110000", ("reg",)),
    "return" : Command("1110001", ()),
    "add3"   : Command("1110010", ("reg", "reg", "reg")),
    "add3i"  : Command("1110011", ("reg", "reg", "const")),
    "sub3"   : Command("1110100", ("reg", "reg", "reg")),
    "sub3i"  : Command("1110101", ("reg", "reg", "const")),
    "and3"   : Command("1110110", ("reg", "reg", "reg")),
    "and3i"  : Command("1110111", ("reg", "reg", "const")),
    "or3"    : Command("1111000", ("reg", "reg", "reg")),
    "or3i"   : Command("1111001", ("reg", "reg", "const")),
    "xor3"   : Command("1111010", ("reg", "reg", "reg")),
    "xor3i"  : Command("1111011", ("reg", "reg", "const")),
    "asr3"   : Command("1111100", ("reg", "reg", "shiftval")),
    "rese1"  : Command("1111101", ()),
    "rese2"  : Command("1111110", ()),
    "rese3"  : Command("1111111", ()) }

conditions ={\
    "eq"  : Condition("000"),
    "z"   : Condition("000"),
    "neq" : Condition("001"),
    "nz"  : Condition("001"),
    "sgt" : Condition("010"),
    "slt" : Condition("011"),
    "gt"  : Condition("100"),
    "ge"  : Condition("101"),
    "nc"  : Condition("101"),
    "lt"  : Condition("110"),
    "c"   : Condition("110"),
    "le"  : Condition("111")}

conteurs = {\
    "pc" : Counter("00"),
    "sp" : Counter("01"),
    "a0" : Counter("10"),
    "a1" : Counter("11")}


NB_REG = 8
NB_BIT_REG = math.ceil(math.log(NB_REG, 2))
# Longueur en base 2 de NB_REG.

def binary_repr(n, k):
    """Given n an unsigned int, it return it's binary representation on k bits"""


def asm_reg(s):
    "converts the string s into its encoding"

    assert len(s) >= 2
    assert s[0]   == 'r'

    val = int(s[1:])
    # If the value is inccorect, it will raise an error.

    assert 0 =< val < NB_REG

    return binary_repr(val, NB_BIT_REG) 
    # thanks stack overflow. The 3 is the number of bits


def asm_remove_comments(s):
    return s.split(";")[0]


def asm_line(s):
    cmds = asm_remove_comments(s).split()

    if len(cmds) == 0:
        return

    cmd = command[comds[0]]
    args = cmds[1:]

    linecode = []
    linecode.append(cmd.opcode)
 
    assert len(cmd.operands) == len(args)

    encounter_address = False

    for value_type, value in zip(cmd.operands, args):

        if value_type == "reg":
            linecode.append(asm_reg(value))

        elif value_type == "const":
            linecode.append(asm_const(value))

        elif value_type == "dir":
            linecode.append(asm_dir(value))

        elif value_type == "shiftval":
            linecode.append(asm_shiftval(value))

        elif value_type == "ctr":
            linecode.append(asm_ctr(value))

        elif value_type == "size":
            linecode.append(asm_size(value))

        elif value_type == "addr":
            encounter_address = True
            linecode.append(asm_addr(value))

        else:
            raise ValueError("Unknow value type : {}".format(value_type))


    return encounter_address, "".join(linecode)

def asm_doc(s):
    bitcode = []
    for line_nb, line in enumerate(s.split("\n"), 1): 
        # For each line, enumeration begin at 1.
        try:
            bitcode.append(asm_line(line)[1])
        except:
            print("/!\ error at line {}".format(line_nb))
            print(line)
            print()
            raise

