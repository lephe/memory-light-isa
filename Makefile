#! /usr/bin/make -f

all:
	(cd prog; make all)
	(cd emu; make emu)
	(cd chip8; make)

clean:
	(cd prog; make clean)
	(cd emu; make distclean)
	(cd chip8; make clean)
