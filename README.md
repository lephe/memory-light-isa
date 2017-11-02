# ASM 2017

There is two main components in this repo : The first one is the Emulator (directory `/emu`), all emulation, debug stuff is there. The second one is the compilation (directory `/compiler`)

## Compiler (`/compiler`)

### Usage

Use `python -m compiler -h` to display help.

Basic use is `python -m compiler -h (--stdout | --output=file) (-m|-t|-b) file.ps`. The `--stdout` option will print the program, whereas `--output=file` will write in the file the output. The three options -m -t and -b specify the back-end. -m print mnemonics, -t print the debug version, and -b the binary version. There is a optionnal command, --huffman, that will generate an opcode.txt file where is stored the binary for each mnemonics.

### Troubleshooting

Please add an issue if you find any bugs.


## Emulator (`/emu`)

Building the emulator consists in running `make` in the `/emu` directory. See `doc/emu_troubleshooting.md` if anything goes wrong at compile time or at execution time.

The emulator runs a ncurses interface. The minimal usable terminal geometry is 96 * 32. The recommended geometry is anything beyond 128 * 40; on smaller terminals some information (such as instruction addresses) are hidden so that everything fits on the screen.

TODO - Document execution modes, and program invocation

The debugger runs a command-line which has an integrated help. Please refer to this help for details about how to use the debugger.

