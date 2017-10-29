import argparse

from .compileuh import compile_asm

if __name__ == "__main__":
    s = """let r4 2
    add r1 r2
    push 64 r4
    sub r3 5
    getctr sp r0
    cmp r3 r5
    shift left r5 3
    push 0x40 r0
    and r2 3
    return"""

    compile_asm(s)
