#! /usr/bin/make -f

src = $(wildcard *.s)
obj = $(src:.s=.bin)

all: $(obj)

%.bin: %.s
	sed '/[:]/ d' $< > /tmp/$<
	../compileme -b /tmp/$< --output=$@
