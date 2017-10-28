#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdint.h>

#include <defs.h>
#include <errors.h>
#include <memory.h>
#include <cpu.h>
#include <debugger.h>

//---
//	Command-line parsing
//---

typedef struct
{
	enum {
		mode_run	= 1,
		mode_debug	= 2,
		mode_graphical	= 3,
	} mode		:8;
	uint help	:8;

	uint32_t	memsize;
	uint32_t	stack;

	const char *filename;
} opt_t;

/* sizetoi() -- parse nonnull integers with 'k' and 'M' suffixes */
uint32_t sizetoi(const char *arg)
{
	if(!isdigit(arg[0])) return 0;

	uint32_t value = 0;
	while(isdigit(*arg)) value = value * 10 + (*arg - '0'), arg++;

	if(!arg[0]) return value;
	if(arg[1]) return 0;
	if(arg[0] == 'k') return value << 10;
	if(arg[0] == 'M') return value << 20;
	return 0;
}

/* parse_args() -- parse command-line options, filling an opt_t structure */
void parse_args(int argc, char **argv, opt_t *opt)
{
	/* Initialize structure */
	opt->mode = opt->help = 0;
	opt->memsize = opt->stack = 0;
	opt->filename = NULL;

	error_clear();

	for(int i = 1; i < argc; i++)
	{
		const char *arg = argv[i];

		/* Mode arguments */
		int newmode = 0;
		if(!strcmp(arg, "-r") || !strcmp(arg, "-run"))
			newmode = mode_run;
		if(!strcmp(arg, "-d") || !strcmp(arg, "-debug"))
			newmode = mode_debug;
		if(!strcmp(arg, "-g") || !strcmp(arg, "-graphical"))
			newmode = mode_graphical;

		if(newmode)
		{
			if(opt->mode) error("unexpected mode: '%s'", arg);
			else opt->mode = newmode;
		}

		/* Memory-related arguments */
		else if(!strcmp(arg, "--stack-addr"))
		{
			arg = argv[++i];
			if(!arg) error("expected value after --stack-addr");
			else opt->stack = sizetoi(arg);
			if(!opt->stack)
				error("invalid stack address: '%s'", arg);
		}
		else if(!strcmp(arg, "--memory-size"))
		{
			arg = argv[++i];
			if(!arg) error("expected value after --memory-size");
			else opt->memsize = sizetoi(arg);
			if(!opt->memsize)
				error("invalid memory size: '%s'", arg);
		}

		/* Help message */
		else if(!strcmp(arg, "--help")) opt->help = 1;

		/* Unknown predicates */
		else if(arg[0] == '-') error("unknown predicate '%s'", arg);

		/* Other arguments are considered to be the input file name */
		else
		{
			if(opt->filename)
				error("unexpected file name: '%s'", arg);
			else opt->filename = arg;
		}
	}

	error_check();
}

//---
//	Main program
//---

const char *help_string =
"emu - an emulator and debugger for a fictional processor\n"
"usage: %s [-run|-debug|-graphical] <binary> [options...]\n\n"

"Available run modes:\n"
"  -r | -run              Normal running mode with output to terminal\n"
"  -d | -debug            Step-by-step execution with debug information\n"
"  -g | -graphical        Same as -run, but also emulate the screen\n\n"

"Available options:\n"
"  --stack-addr  <addr>   Set the bottom stack address (default 1M)\n"
"  --memory-size <size>   Set the total memory size (default 4M)\n"
"Both constants may use the 'k' or 'M' suffixes (eg. 16k or 2M).\n";

memory_t *mem = NULL;
cpu_t *cpu = NULL;

void help(const char **argv)
{
	printf(help_string, argv[0]);
	exit(0);
}

void quit(void)
{
	if(cpu) cpu_destroy(cpu);
	if(mem) memory_destroy(mem);
}

int main(int argc, char **argv)
{
	atexit(quit);

	/* Parse command-line arguments */
	opt_t opt;
	parse_args(argc, argv, &opt);

	if(argc == 1 || opt.help) help((const char **)argv);
	if(!opt.filename) fatal("no input file");

	if(opt.mode == mode_run)
		fatal("cannot honor run mode x_x (TODO)");
	if(opt.mode == mode_graphical)
		fatal("cannot honor graphical mode x_x (TODO)");

	/* Allocate a virtual memory and load the program into it */
	mem = memory_new(opt.memsize, opt.stack, 0);
	memory_load(mem, opt.filename);

	/* Create a CPU and give it the memory */
	cpu = cpu_new(mem);

	/* Start the debugger */
	debugger(opt.filename, cpu);

	return 0;
}
