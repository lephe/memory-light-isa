#! /usr/bin/make -f

all:
	(cd prog; make all)
	(cd emu; make emu)

clean:
	(cd prog; make clean)
	(cd emu; make distclean)
