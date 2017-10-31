import argparse

from .compileuh import compile_asm
from .back_end import MemonicBackEnd, CleartextBitcodeBackEnd
from .back_end import BinaryBitcodeBackEnd

if __name__ == "__main__":

    parser = argparse.ArgumentParser()
    parser.add_argument("filename")

    group_out = parser.add_mutually_exclusive_group()
    group_out.add_argument("-o", "--output", type=str,
                           dest="output",
                           help="Specify the output")
    group_out.add_argument("--stdout", action="store_true",
                           dest="output", default=True,
                           help="Print output")

    group_backend = parser.add_mutually_exclusive_group(required=True)
    group_backend.add_argument("-m", "--menmonics", action="store_const",
                               dest="back_end", const=MemonicBackEnd,
                               help="output asr menomonics")
    group_backend.add_argument("-t", "--teacher", action="store_const",
                               dest="back_end", const=CleartextBitcodeBackEnd,
                               help="output asr like the teacher")
    group_backend.add_argument("-b", "--binary", action="store_const",
                               dest="back_end", const=BinaryBitcodeBackEnd,
                               help="output in binary")

    parser.add_argument('--huffman', action="store_true",
                        help="generate the huffman tree")

    args = parser.parse_args()
    with open(args.filename) as file:
        s = file.read()

        out = compile_asm(s, back_end=args.back_end,
                          generate_tree=args.huffman)

        if args.output is True:
            out.to_output()
