#! /usr/bin/make -f

src = $(wildcard *.s)
obj = $(src:.s=.bin)

all: $(obj)

%.bin: %.s
	../asm.py -b $< -o $@

clean:
	@ rm -f *.bin
	@ rm -f *.debug
