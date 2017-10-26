import enum
import collections
import re



Token = collections.namedtuple('Token', ['typ', 'value', 'line', 'column'])

class LexType(enum.Enum):
    OPERATION = enum.auto()
    REGISTER  = enum.auto()
    DIRECTION = enum.auto()
    NUMBER    = enum.auto()
    COMMENT   = enum.auto()
    CONDITION = enum.auto()
    COUNTER   = enum.auto()
    NEWLINE   = enum.auto()
    SKIP      = enum.auto()
    MISMATCH  = enum.auto()
    ENDFILE   = enum.auto()
    LABEL     = enum.auto()

def inv_dict_list(d):
    inv_d = dict()

    for k1, v in d.items():
        for k2 in v:
            inv_d[k2] = k1

    return inv_d


possible_transition = { \
    "add" :    ["add2", "add2i", "add3", "add3i"],
    "and" :    ["and2", "and2i", "and3", "and3i"],
    "sub" :    ["sub2", "sub2i", "sub3", "sub3i"],
    "or"  :    ["or2", "or2i", "or3", "or3i"],
    "xor" :    ["xor3", "xor3i"],
    "cmp" :    ["cmp", "cmpi"],
    "let" :    ["let", "leti"],
    "shift" :  ["shift"],
    "readze" : ["readze"],
    "readse" : ["readse"],
    "jump" :   ["jump", "jumpif"],
    "write" :  ["write"],
    "call" :   ["call"],
    "setctr" : ["setctr"],
    "getctr" : ["getctr"],
    "push" :   ["psuh"],
    "return" : ["return"],
    "asr" :    ["asr"]}

inverse_possible_transition = inv_dict_list(possible_transition)



s = '''
    add3 r3 r5 r6
    add2i r1 343
    add3 r3 r5 r6
    add2i r1 343
    add3 r3 r5 r6
    add2i r1 343
    add3 r3 r5 r6
    add2i r1 34
'''




class ValueType(enum.Enum):
    REGISTER   = enum.auto()
    DIRECTION  = enum.auto()
    CONDITION  = enum.auto()
    MEMCOUNTER = enum.auto()
    UCONSTANT  = enum.auto() # Unsigned Constant
    SCONSTANT  = enum.auto() # Signed Constant
    RADDRESS   = enum.auto() # Relative address
    AADDRESS   = enum.auto() # Absolute address
    SHIFTVAL   = enum.auto()
    SIZE       = enum.auto()


VT = ValueType
asr_specs = {\
    "add2"   : (VT.REGISTER, VT.REGISTER),
    "add2i"  : (VT.REGISTER, VT.UCONSTANT),
    "add3"   : (VT.REGISTER, VT.REGISTER, VT.REGISTER),
    "add3i"  : (VT.REGISTER, VT.REGISTER, VT.UCONSTANT),

    "sub2"   : (VT.REGISTER, VT.REGISTER),
    "sub2i"  : (VT.REGISTER, VT.UCONSTANT),
    "sub3"   : (VT.REGISTER, VT.REGISTER, VT.REGISTER),
    "sub3i"  : (VT.REGISTER, VT.REGISTER, VT.UCONSTANT),

    "cmp"    : (VT.REGISTER, VT.REGISTER),
    "cmpi"   : (VT.REGISTER, VT.SCONSTANT),
    
    "let"    : (VT.REGISTER, VT.REGISTER),
    "leti"   : (VT.REGISTER, VT.SCONSTANT),
    
    "shift"  : (VT.DIRECTION, VT.REGISTER, VT.SHIFTVAL),
    
    "readze" : (VT.MEMCOUNTER, VT.SIZE, VT.REGISTER),
    
    "readse" : (VT.MEMCOUNTER, VT.SIZE, VT.REGISTER),
    
    "jump"   : (VT.RADDRESS,),
    "jumpif" : (VT.CONDITION, VT.RADDRESS),

    "or2"    : (VT.REGISTER, VT.REGISTER),
    "or2i"   : (VT.REGISTER, VT.SCONSTANT),
    "or3"    : (VT.REGISTER, VT.REGISTER, VT.REGISTER),
    "or3i"   : (VT.REGISTER, VT.REGISTER, VT.SCONSTANT),

    "and2"   : (VT.REGISTER, VT.REGISTER),
    "and2i"  : (VT.REGISTER, VT.SCONSTANT),
    "and3"   : (VT.REGISTER, VT.REGISTER, VT.REGISTER),
    "and3i"  : (VT.REGISTER, VT.REGISTER, VT.SCONSTANT),

    "write"  : (VT.MEMCOUNTER, VT.SIZE, VT.REGISTER),
    "call"   : (VT.AADDRESS,),
    "setctr" : (VT.MEMCOUNTER, VT.REGISTER),
    "getctr" : (VT.MEMCOUNTER, VT.REGISTER),
    "push"   : (VT.REGISTER,),
    "return" : (),
    
    "xor3"   : (VT.REGISTER, VT.REGISTER, VT.REGISTER),
    "xor3i"  : (VT.REGISTER, VT.REGISTER, VT.SCONSTANT),

    "asr3"   : (VT.REGISTER, VT.REGISTER, VT.SHIFTVAL),

    "reserved1"  : (),
    "reserved2"  : (),
    "reserved3"  : ()}

del VT

