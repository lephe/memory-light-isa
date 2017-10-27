#! /usr/bin/make -f
#
#	Architecture, Systems, Networks 2017 course at ENS Lyon
#	An emulator and debugger for the fictional processor
#
#---

target	= emu
src	= $(wildcard src/*.c)
obj	= $(src:src/%=build/%.o)

cflags	= -Wall -Wextra -std=c11 -I include
dflags	= -MT $@ -MMD -MP -MF $(@:.o=.d)
lflags	= $(cflags) -lncursesw

$(target): $(obj)
	gcc $^ -o $@ $(lflags)

build/%.o: src/% | build
	gcc -c $< -o $@ $(cflags) $(dflags)

build:
	@ mkdir -p $@

# Dependency files. Force building when they don't exist, and keep them.
build/%.d:;
.PRECIOUS: build/%.d
# Include header dependencies
-include $(wildcard build/*.d)

clean:
	@ rm -f build/*.o
distclean: clean
	@ rm -rf build
	@ rm -f $(target)

.PHONY: clean distclean
