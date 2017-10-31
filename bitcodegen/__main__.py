import argparse

from .compileuh import compile_asm

if __name__ == "__main__":

    parser = argparse.ArgumentParser()
    parser.add_argument('filename')
    args = parser.parse_args()
    with open(args.filename) as file:
        s = file.read()

        compile_asm(s)
