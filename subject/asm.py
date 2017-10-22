#!/usr/bin/env python

# This program assembles source assembly code into a bit string.
# The bit string includes spaces and newlines for readability,
# these should be ignored by the simulator when it reads the corresponding file.

import os
import sys
import re
import string
import argparse
from numpy import binary_repr

line=0 # global variable to make error reporting easier
current_addr=0 # idem
labels={} # global because shared between the two passe

def error(e):
    raise BaseException("Error at line " + str(line) + " : " + e)



# All the asm_xxxx functions are helper functions that parse an operand and convert it into its binary encoding.
# Many are still missing

def asm_reg(s):
    "converts the string s into its encoding"
    if s[0]!='r':
        error("invalid register: " + s)
    val = int(s[1:]) # this removes the "r". TODO catch exception here
    if val<0 or val>7:
        error("invalid register: " + s)
    else:
        return binary_repr(val,3) + ' '  # thanks stack overflow. The 3 is the number of bits



def asm_addr_signed(s):
    "converts the string s into its encoding"
    # Is it a label or a constant? 
    if (s[0]>='0' and s[0]<='9') or s[0]=='-' or s[0]=='+': # TODO what it takes to catch hexa here
        val=int(s) # TODO  catch exception here
        # The following is not very elegant but easy to trust
        if val>=-128 and val<= 127:
            return '0 ' + binary_repr(val, 8)
        elif val>=-32768 and val<= 32767:
            return '10 ' +  binary_repr(val, 16)
        elif val>=-(1<<31) and val<= (1<<31)-1:
            return '110 ' + binary_repr(val, 32)
        else:
            return '111 ' +  binary_repr(val, 64)
    else:
        error("Fixme! labels currently unsupported")
    
    


        
def asm_const_unsigned(s):
    "converts the string s into its encoding"
    # Is it a label or a constant? 
    if (s[0]>='0' and s[0]<='9') or s[0:2]=="0x":
        val=int(s,0) # TODO  catch exception here
        # The follwing is not very elegant but easy to trust
        if val==0 or val==1:
            return '0 ' + str(val)
        elif val< 256:
            return '10 ' + binary_repr(val, 8)
        elif  val< (1<<32):
            return '110 ' + binary_repr(val, 32)
        else:
            return '111 ' +  binary_repr(val, 64)
    else:
        error("Expecting a constant, got " + s)

        
def asm_const_signed(s):
    "converts the string s into its encoding"
    # begin sabote
    # end sabote
    
def asm_shiftval(s):
    "converts the string s into its encoding"
    # begin sabote
    # end sabote
           

def asm_condition(cond):
    """converts the string cond into its encoding in the condition code. """
    condlist = {"eq":"000", "z":"000",  "neq":"001",  "nz":"001",  "sgt":"010",  "slt":"011",  "gt":"100",  "ge":"101",  "nc":"101",  "lt":"110",  "c":"110",  "le":"111"}
    if cond in condlist:
        val = condlist[cond]
        return val + " "
    else:
        error("Invalid condition: " + cond)


def asm_counter(ctr):
    """converts the string ctr into its encoding. """
    codelist = {"pc":"00", "sp":"01",  "a0":"10",  "a1":"11",  "0":"00",  "1":"01",  "2":"10",  "3":"11"}
    if ctr in codelist:
        val = codelist[ctr]
        return val + " "
    else:
        error("Invalid counter: " + cond)



def asm_size(s):
    """converts the string s into its encoding. """
    codelist = {"1":"00", "4":"01",  "8":"100",  "16":"101",  "32":"110",  "64":"111"}
    if s in codelist:
        val = codelist[s]
        return val + " "
    else:
        error("Invalid size: " + size)


def asm_pass(iteration, s_file):
    global line
    global labels
    global current_address
    code =[] # array of strings, one entry per instruction
    print "\n PASS " + str(iteration)
    current_address = 0
    source = open(s_file)
    for source_line in source:
        instruction_encoding="" 
        print "processing " + source_line[0:-1] # just to get rid of the final newline

        # if there is a comment, get rid of it
        index = str.find(";", source_line)
        if index !=-1:
            source_line = source_line[:index]

        # split the non-comment part of the line into tokens (thanks Stack Overflow) 
        tokens = re.findall('[\S]+', source_line) # \S means: any non-whitespace
        # print tokens # to debug

        # if there is a label, consume it
        if tokens:
            token=tokens[0]
            if token[-1] == ":": # last character
                label = token[0: -1] # all the characters except last one
                labels[label] = current_address
                tokens = tokens[1:]

        # now all that remains should be an instruction... or nothing
        if tokens:
            opcode = tokens[0]
            token_count = len(tokens)
            if opcode == "add2" and token_count==3:
                    instruction_encoding = "0000 " + asm_reg(tokens[1]) + asm_reg(tokens[2])
            if opcode == "add2i" and token_count==3:
                    instruction_encoding = "0001 " + asm_reg(tokens[1]) + asm_const_unsigned(tokens[2])
            # Here, a lot of constructive copypaste, for instance
            if opcode == "jump" and token_count==2:
                    instruction_encoding = "1010 " + asm_addr_signed(tokens[1])
         #begin sabote
            #end sabote
                    
            # If the line wasn't assembled:
            if instruction_encoding=="":
                error("don't know what to do with: " + source_line)
            else:
                # get rid of spaces. Thanks Stack Overflow
                compact_encoding = ''.join(instruction_encoding.split()) 
                instr_size = len(compact_encoding)
                # Debug output
                print "... @" + str(current_address) + " " + binary_repr(current_address,16) + "  :  " + compact_encoding
                print  "                          "+  instruction_encoding+ "   size=" + str(instr_size)    
                current_address += instr_size

                
        line += 1
        code.append(instruction_encoding)
    source.close()
    return code




#/* main */
if __name__ == '__main__':

    argparser = argparse.ArgumentParser(description='This is the assembler for the ASR2017 processor @ ENS-Lyon')
    argparser.add_argument('filename', help='name of the source file.  "python asm.py toto.s" assembles toto.s into toto.obj')

    options=argparser.parse_args()
    filename = options.filename
    basefilename, extension = os.path.splitext(filename)
    obj_file = basefilename+".obj"
    code = asm_pass(1, filename) # first pass essentially builds the labels

     # code = asm_pass(2, filename) # second pass is for good, but is disabled now

    # statistics
    print "Average instruction size is " + str(1.0*current_address/len(code))
    
    outfile = open(obj_file, "w")
    for instr in code:
        outfile.write(instr)
        outfile.write("\n")


    outfile.close()
