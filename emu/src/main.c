#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdint.h>

#include <defs.h>
#include <errors.h>

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

/* sizetoi() -- parse nonnull integers supporting 'k' and 'M' */
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

int main(int argc, char **argv)
{
	/* Parse command-line options */

	opt_t opt;
	parse_args(argc, argv, &opt);

	if(argc == 1 || opt.help)
	{
		printf(help_string, argv[0]);
		return 0;
	}

	if(!opt.filename) fatal("no input file");

	/* Check the memory size or set defaults */

	if(!opt.stack) opt.stack = 1 << 20;
	if(!opt.memsize) opt.memsize = 4 << 20;
	uint32_t vram_size = 1 << 20;

	/* Open and load the binary file into memory.
	   Also check that the provided memory geometry is consistent */

	FILE *fp = fopen(opt.filename, "r");
	if(!fp) fatal("# cannot open '%s': ", opt.filename);

	unsigned long size;
	fseek(fp, 0, SEEK_END);
	size = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	error_clear();

	if(opt.stack > opt.memsize)
		fatal("inconsistent memory layout, no space left for stack");
	if(opt.stack + vram_size > opt.memsize)
		fatal("inconsistent memory layout, no space left for video "
			"memory");
	if(opt.stack + vram_size == opt.memsize)
		warn("no space left for the data segment");
	if(size > opt.stack)
		error("program is too large to fit in the code segment (%d > "
			"%d)", size, opt.stack);

	error_check();

	fclose(fp);
	return 0;
}
