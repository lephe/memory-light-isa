#! /usr/bin/make -f

src = $(wildcard *.s) $(wildcard *.ps)
obj = $(patsubst %.ps,%.bin,$(src:.s=.bin))


all: $(obj)

%.bin: %.s
	../asm.py -b $< -o $@
%.bin: %.ps
	../asm.py -b $< -o $@

clean:
	@ rm -f *.bin
	@ rm -f *.debug
