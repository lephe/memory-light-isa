#! /usr/bin/make -f

src = $(wildcard *.s)
obj = $(src:.s=.bin)

all: $(obj)

%.bin: %.s
	sed '/[:]/ d' $< > /tmp/$<
	python3 ../bitcodegen/myasm.py /tmp/$*
	mv /tmp/$*.{bin,debug} $(dir $@)
