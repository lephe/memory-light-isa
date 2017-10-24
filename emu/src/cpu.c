#include <stdlib.h>
#include <errors.h>
#include <cpu.h>

/* cpu_new() -- create a cpu and associate it with a given memory object */
cpu_t *cpu_new(memory_t *mem)
{
	if(!mem) ifatal("cpu_new(): NULL memory argument");

	cpu_t *cpu = malloc(sizeof *cpu);
	if(!cpu) ifatal("# cpu_new(): cannot allocate memory");

	cpu->mem = mem;

	/* Initialize registers and flags */
	for(int i = 0; i < 8; i++) cpu->r[i] = 0x00000000;
	cpu->z = 0;
	cpu->c = 0;
	cpu->n = 0;

	/* Initialize pointers to memory */
	cpu->ptr[PC] = 0x000000;
	cpu->ptr[SP] = mem->stack;
	cpu->ptr[A0] = mem->data;
	cpu->ptr[A1] = mem->data;

	return cpu;
}

/* cpu_destroy() -- destroy a cpu_t object (not its memory) */
void cpu_destroy(cpu_t *cpu)
{
	free(cpu);
}
