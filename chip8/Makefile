src = $(wildcard *.s)

chip8.bin: $(src)
	../asm.py -b main.s -o $@

clean:
	@ rm -f chip8.bin
