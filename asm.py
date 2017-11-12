#!/bin/python3

import argparse

from compiler.compileuh import compile_asm
from compiler.back_end import MemonicBackEnd, CleartextBitcodeBackEnd
from compiler.back_end import BinaryBitcodeBackEnd
from compiler.labels import LabelsClearTextBackEnd, LabelsBinaryBackEnd

import sys

sys.argv[0] = "compileme"


if __name__ == "__main__":

    parser = argparse.ArgumentParser()
    parser.add_argument("filename", help='name of the source file.  "python asm.py toto.s" assembles toto.s into toto.obj')

    group_out = parser.add_mutually_exclusive_group()
    group_out.add_argument("-o", "--output", type=str,
                           dest="output",
                           help="Specify the output")
    group_out.add_argument("--stdout", action="store_true",
                           dest="output",
                           help="Print output")

    group_backend = parser.add_mutually_exclusive_group()
    group_backend.add_argument("-m", "--menmonics", action="store_const",
                               dest="back_end", const=MemonicBackEnd,
                               help="output asr menomonics")
    """group_backend.add_argument("-t", "--teacher", action="store_const",
                                           dest="back_end", const=CleartextBitcodeBackEnd,
                                           help="output asr like the teacher")
    group_backend.add_argument("-b", "--binary", action="store_const",
                                           dest="back_end", const=BinaryBitcodeBackEnd,
                                           help="output in binary")"""
    group_backend.add_argument("-t", "--teacher", action="store_const",
                               dest="back_end", const=LabelsClearTextBackEnd,
                               help="Clear text with labels")
    group_backend.add_argument("-b", "--binary", action="store_const",
                               dest="back_end", const=LabelsBinaryBackEnd,
                               help="Output in binary with labels")

    parser.add_argument('--huffman', action="store_true",
                        help="generate the huffman tree")

    args = parser.parse_args()
    with open(args.filename) as file:
        s = file.read()

        if args.back_end is None:
            args.back_end = LabelsClearTextBackEnd

        out = compile_asm(s, back_end=args.back_end,
                          generate_tree=args.huffman)

        # print(args.output)
        # print(args.filename.rsplit(".", maxsplit=1))

        if args.output is True:
            out.to_output()

        if args.output is None:
            start = args.filename.rsplit(".", maxsplit=1)[0]
            if args.back_end is MemonicBackEnd:
                end = "obj.s"
            elif args.back_end is LabelsClearTextBackEnd:
                end = "obj"
            elif args.back_end is LabelsBinaryBackEnd:
                end = "bin"

            wfilename = f"{start}.{end}"
            out.to_file(wfilename)

        elif type(args.output) is str:
            out.to_file(args.output)
