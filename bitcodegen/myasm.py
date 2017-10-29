#!/usr/bin/env python3

from collections import namedtuple
import re
import math

Command = namedtuple("Command", ["opcode", "operands"])
Condition = namedtuple("Condition", ["opcode"])


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

    "pop"    : Command("1001001", ("size", "reg")),

    "readse" : Command(  "10011", ("ctr", "size", "reg")),
    "jump"   : Command(   "1010", ("addr_signed",)),
    "jumpif" : Command(   "1011", ("cond", "addr_signed")),
    "or2"    : Command( "110000", ("reg", "reg")),
    "or2i"   : Command( "110001", ("reg", "const")),
    "and2"   : Command( "110010", ("reg", "reg")),
    "and2i"  : Command( "110011", ("reg", "const")),
    "write"  : Command( "110100", ("ctr", "size", "reg")),
    "call"   : Command( "110101", ("addr_signed",)),
    "setctr" : Command( "110110", ("ctr", "reg")),
    "getctr" : Command( "110111", ("ctr", "reg")),
    "push"   : Command("1110000", ("size", "reg",)),
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



class TokenError(ValueError):
    pass

NB_REG = 8
NB_BIT_REG = math.ceil(math.log(NB_REG, 2))
# Longueur en base 2 de NB_REG.

def binary_repr(n, k, signed=False):
    """Given n an int, it return it's binary representation on k bits"""

    if signed:
        if n not in range(-2**(k-1), 2**(k-1)):
            raise TokenError("Number not in range")

        n = (2**k + n) % 2**k

    unfilled = bin(n)[2:]
    
    if len(unfilled) > k:
        raise TokenError("Too long binary")

    return "0" * (k-len(unfilled)) + unfilled


re_reg = re.compile(r"^r([0-9]+)$")
def asm_reg(s):

    res = re_reg.findall(s)

    if len(res) != 1:
        raise TokenError("invalid register syntax")

    val = int(res[0]) # No possible error here.

    assert 0 <= val < NB_REG
    
    return binary_repr(val, NB_BIT_REG)

re_const = re.compile(r"^([+-]?0x[0-9A-Fa-f]+)|([+-]?[0-9]+)$")
def asm_const(s):
    res = re_const.findall(s)
    # res in the form [("0x12AefF3", "120328')]

    if len(res) != 1:
        raise TokenError("invalid constant syntax")

    # Reading the regular expression
    if len(res[0][0]) > 0: # hexa
        val = int(res[0][0], 0x10)
    elif len(res[0][1]) > 0: # decimal
        val = int(res[0][1])
    else:
        raise TokenError("invalid constant syntax : empty constant")


    # Encoding the prefix-free ALU constant

    if val in range(2**1):
        # Range in NOT a list in python3
        return   "0" + binary_repr(val, 1)
    elif val in range(2**8):
        return  "10" + binary_repr(val, 8)
    elif val in range(2**32):
        return "110" + binary_repr(val, 32)
    elif val in range(2**64):
        return "111" + binary_repr(val, 64)
    else:
        raise TokenError("invalid constant : Not in range")


re_dir = re.compile(r"(left)|(right)")
def asm_dir(s):
    res = re_dir.findall(s)
    # res in the form [("left", "right")]

    if len(res) != 1:
        raise TokenError("invalid direction syntax")

    if len(res[0][0]) > 0 : # left
        val = 0
    elif len(res[0][1]) > 0 : # right
        val = 1
    else:
        raise TokenError("invalid direction syntax : empty")

    return binary_repr(val, 1)


re_shiftval = re.compile(r"^(0x[0-9A-Fa-f]+)|([0-9]+)$")
def asm_shiftval(s):
    res = re_shiftval.findall(s)
    # res in the form [("0x12AefF3", "120328')]

    if len(res) != 1:
        raise TokenError("invalid shiftval syntax")

    # Reading the regular expression
    if len(res[0][0]) > 0: # hexa
        val = int(res[0][0], 0x10)
    elif len(res[0][1]) > 0: # decimal
        val = int(res[0][1])
    else:
        raise TokenError("invalid shiftval syntax : empty shiftval")

    if val == 1:
        return binary_repr(val, 1)
    elif val in range(2**6):
        return "0" + binary_repr(val, 6)
    else:
        raise TokenError("invalid shiftval : Not in range")


Counter = namedtuple("Counter", ["opcode"])
ctr = {\
    "pc" : Counter("00"),
    "sp" : Counter("01"),
    "a0" : Counter("10"),
    "a1" : Counter("11")}
re_ctr = re.compile(r"(pc|sp|a0|a1)")
def asm_ctr(s):
    res = re_ctr.findall(s)

    if len(res) != 1:
        raise TokenError("invalid counter syntax")

    val = res[0]

    return ctr[val].opcode


re_size = re.compile(r"^(0x[0-9A-Fa-f]+)|([0-9]+)$")
def asm_size(s):
    res = re_size.findall(s)

    if len(res) != 1:
        raise TokenError("invalid size syntax")

    if len(res[0][0]) > 0:
        val = int(res[0][0], 0x10)
    elif len(res[0][1]) > 0:
        val = int(res[0][1])
    else:
        raise TokenError("invalid size syntax : empty size")


    if val == 1:
        return "00"
    elif val == 4:
        return "01"
    elif val == 8:
        return "100"
    elif val == 16:
        return "101"
    elif val == 32:
        return "110"
    elif val == 64:
        return "111"
    else:
        raise TokenError("invalid size : not in range")


re_addr_signed = re.compile(r"^([+-]?0x[0-9A-Fa-f]+)|([+-]?[0-9]+)$")
def asm_addr_signed(s):
    res = re_addr_signed.findall(s)
    # res in the form [("0x12AefF3", "120328')]

    if len(res) != 1:
        raise TokenError("invalid signed address syntax")

    # Reading the regular expression
    if len(res[0][0]) > 0: # hexa
        val = int(res[0][0], 0x10)
    elif len(res[0][1]) > 0: # decimal
        val = int(res[0][1])
    else:
        raise TokenError("invalid signed address syntax : empty address")

    if val in range(-2**7, 2**7):
        return "0" + binary_repr(val, 8, signed=True)
    elif val in range(-2**15, 2**15):
        return "10" + binary_repr(val, 16, signed=True)
    elif val in range(-2**31, 2**31):
        return "110" + binary_repr(val, 32, signed=True)
    elif val in range(-2**63, 2**63):
        return "111" + binary_repr(val, 64, signed=True)
    else:
        raise TokenError("invalid unsigned address : not in range")


re_addr_unsigned = re.compile(r"^(0x[0-9A-Fa-f]+)|([0-9])?$")
def asm_addr_unsigned(s):
    res = re_addr_unsigned.findall(s)
    # res in the form [("0x12AefF3", "120328')]

    if len(res) != 1:
        raise TokenError("invalid unsigned address syntax")

    # Reading the regular expression
    if len(res[0][0]) > 0: # hexa
        val = int(res[0][0], 0x10)
    elif len(res[0][1]) > 0: # decimal
        val = int(res[0][1])
    else:
        raise TokenError("invalid unsigned address syntax : empty address")

    if val in range(2**8):
        return "0" + binary_repr(val, 8)
    elif val in range(2**16):
        return "10" + binary_repr(val, 16)
    elif val in range(2**32):
        return "110" + binary_repr(val, 32)
    elif val in range(2**64):
        return "111" + binary_repr(val, 64)
    else:
        raise TokenError("invalid unsigned address : not in range")

re_cond = re.compile(r"(eq|z|neq|nz|sgt|slt|gt|ge|nc|lt|c|le)")
def asm_cond(s):
    res = re_cond.findall(s)
    return conditions[res[0]]



def asm_remove_comments(s):
    return s.split(";")[0]


def asm_line(s):
    cmds = asm_remove_comments(s).split()

    if len(cmds) == 0:
        return ""

    cmd = commands[cmds[0]]
    args = cmds[1:]

    linecode = []
    linecode.append(cmd.opcode)
    print(linecode)
 
    assert len(cmd.operands) == len(args)

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

        elif value_type == "addr_signed":
            linecode.append(asm_addr_signed(value))

        elif value_type == "addr_unsigned":
            linecode.append(asm_addr_unsigned(value))

        elif value_type == "cond":
            linecode.append(asm_cond(value))

        else:
            raise ValueError("Unknow value type : {}".format(value_type))


    return " ".join(linecode)

def asm_doc(s):
    bitcode = []
    for line_nb, line in enumerate(s.split("\n"), 1): 
        # For each line, enumeration begin at 1.
        try:
            bitcode.append(asm_line(line))
        except:
            print("/!\ error at line {}".format(line_nb))
            print(line)
            print()
            raise

    return "\n".join(bitcode)




if __name__ == '__main__':
    import argparse
    parser = argparse.ArgumentParser()
    parser.add_argument('filename', action='store',
                        type=str,
                        help='name of the source file.  "python asm.py toto.s" assembles toto.s into toto.obj')


    args = parser.parse_args()
    
    with open(args.filename + ".s", "r") as rf:
        res = asm_doc(rf.read())


    with open(args.filename + ".debug", "w+") as wf:
        wf.write(res)


    res = "".join(res.split())
    res = res + "0" * ((8-len(res))%8) # Adding zeros at the end
    bin_ = int(res, 2).to_bytes(len(res)//8, byteorder='big') # Converting to byte object

    with open(args.filename + ".bin", "wb+") as wf:
        wf.write(bin_)
