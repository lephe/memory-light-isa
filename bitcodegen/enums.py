from enum import Enum
import re

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

inverse_possible_transition = dict()

for k1, v in possible_transition.items():
    for k2 in v:
        inverse_possible_transition[k2] = k1

from pprint import pprint



s = '''
    add3 r3 r5 r6
    add2i r1 343
'''

def sub(s, d):
    """ Arguments : s a string, d a dictionnary with regular expressions as keys. Return s with all matches remplaces by the string in the dict"""

    pattern = re.compile('(' + '|'.join(d.keys()) + ')')

    return pattern.sub(lambda x: d[x.group()], s)

def add_global_enum(e):
    """ Add all enum's value to globals"""

    for name, member in e.__members__.items():
        globals()[name] = member

def del_global_enum(e):
    """ Remove all enum's value globals"""
    for name, _ in e.__members__.items():
        del globals()[name]


print(sub(s, inverse_possible_transition))




ValueType = Enum("ValueType", "REGISTER DIRECTION CONDITION MEMCOUNTER UCONSTANT SCONSTANT RADDRESS AADDRESS SIZE SHIFTVAL")

add_global_enum(ValueType)

asr_specs = {\
    "add2"   : (REGISTER, REGISTER),
    "add2i"  : (REGISTER, UCONSTANT),
    "add3"   : (REGISTER, REGISTER, REGISTER),
    "add3i"  : (REGISTER, REGISTER, UCONSTANT),

    "sub2"   : (REGISTER, REGISTER),
    "sub2i"  : (REGISTER, UCONSTANT),
    "sub3"   : (REGISTER, REGISTER, REGISTER),
    "sub3i"  : (REGISTER, REGISTER, UCONSTANT),

    "cmp"    : (REGISTER, REGISTER),
    "cmpi"   : (REGISTER, SCONSTANT),
    
    "let"    : (REGISTER, REGISTER),
    "leti"   : (REGISTER, SCONSTANT),
    
    "shift"  : (DIRECTION, REGISTER, SHIFTVAL),
    
    "readze" : (MEMCOUNTER, SIZE, REGISTER),
    
    "readse" : (MEMCOUNTER, SIZE, REGISTER),
    
    "jump"   : (RADDRESS,),
    "jumpif" : (CONDITION, RADDRESS),

    "or2"    : (REGISTER, REGISTER),
    "or2i"   : (REGISTER, SCONSTANT),
    "or3"    : (REGISTER, REGISTER, REGISTER),
    "or3i"   : (REGISTER, REGISTER, SCONSTANT),

    "and2"   : (REGISTER, REGISTER),
    "and2i"  : (REGISTER, SCONSTANT),
    "and3"   : (REGISTER, REGISTER, REGISTER),
    "and3i"  : (REGISTER, REGISTER, SCONSTANT),

    "write"  : (MEMCOUNTER, SIZE, REGISTER),
    "call"   : (AADDRESS,),
    "setctr" : (MEMCOUNTER, REGISTER),
    "getctr" : (MEMCOUNTER, REGISTER),
    "push"   : (REGISTER,),
    "return" : (),
    
    "xor3"   : (REGISTER, REGISTER, REGISTER),
    "xor3i"  : (REGISTER, REGISTER, SCONSTANT),

    "asr3"   : (REGISTER, REGISTER, SHIFTVAL),

    "reserved1"  : (),
    "reserved2"  : (),
    "reserved3"  : ()}

del_global_enum(ValueType)

print(ValueType.REGISTER)
print(REGISTER)
