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
#include <graphical.h>

//---
//	Command-line parsing
//---

/*
	opt_t structure
	Describes the command-line parameters passed to the program.
*/
typedef struct
{
	uint debugger	:1;
	uint graphical	:1;
	uint help	:1;

	struct {
		uint64_t text;
		uint64_t stack;
		uint64_t data;
		uint64_t vram;
	};

	const char *filename;		/* Emulated binary file */

	/* Load additional file if requested */
	uint64_t load_addr;
	const char *load_file;
} opt_t;

/*
	sizetoi() -- parse non-zero integers with 'k' and 'M' suffixes
	This function reads the provided strings and tries to parse an integer
	on the form:
		[0-9]+[kM]?
	The string must end after the number, thus '34k, World!' is not a valid
	number-representing string. This function returns 0 on error, which is
	definitely not a good error indicator, except if 0 is an invalid value
	in the context. Make sure to check beforehand.

	@arg	arg	String to parse.
	@returns	The value from the string in an integer format.
*/
static uint64_t sizetoi(const char *arg)
{
	if(!isdigit(arg[0])) return 0;

	uint64_t value = 0;
	while(isdigit(*arg)) value = value * 10 + (*arg - '0'), arg++;

	if(!arg[0]) return value;
	if(arg[1]) return 0;
	if(arg[0] == 'k') return value << 10;
	if(arg[0] == 'M') return value << 20;
	return 0;
}

/*
	read_geometry() -- read the geometry info from a string
	Returns non-zero on error.
*/
static int read_geometry(const char *arg, opt_t *opt)
{
	char str[4][64];
	int x = sscanf(arg, "%64[0-9kM]:%64[0-9kM]:%64[0-9kM]:%64[0-9kM]",
		str[0], str[1], str[2], str[3]);
	if(x < 4) return 1;

	opt->text	= sizetoi(str[0]);
	opt->stack	= sizetoi(str[1]);
	opt->data	= sizetoi(str[2]);
	opt->vram	= sizetoi(str[3]);
	return 0;
}

/*
	read_load() -- read the data file info from a string
	Returns non-zero on error.
*/
static int read_load(const char *arg, opt_t *opt)
{
	char str[64];
	int offset;
	int x = sscanf(arg, "%64[0-9kM]:%n", str, &offset);
	if(x != 1) return 1;

	opt->load_addr	= sizetoi(str);
	opt->load_file	= arg + offset;
	return 0;
}

/*
	parse_args() -- parse command-line options into an opt_t object

	@arg	argc	Number of command-line arguments
	@arg	argv	NULL-terminated command-line argument array
	@arg	opt	Option structure to fill with the argument content
*/
static void parse_args(int argc, char **argv, opt_t *opt)
{
	/* Clear structure with default values */
	opt->debugger = opt->graphical = 0;
	opt->help = 0;
	opt->text = opt->stack = opt->data = opt->vram = 0;
	opt->filename = NULL;
	opt->load_addr = 0;
	opt->load_file = NULL;

	error_clear();

	for(int i = 1; i < argc; i++)
	{
		const char *arg = argv[i];

		/* Mode arguments */
		if(!strcmp(arg, "-r") || !strcmp(arg, "--run"))
			opt->debugger = 0;
		else if(!strcmp(arg, "-d") || !strcmp(arg, "--debug"))
			opt->debugger = 1;
		else if(!strcmp(arg, "-g") || !strcmp(arg, "--graphical"))
			opt->graphical = 1;

		/* Memory geometry */
		else if(!strcmp(arg, "--geometry"))
		{
			arg = argv[++i];
			if(!arg) error("expected value after --stack-addr");
			else if(read_geometry(arg, opt))
				error("invalid geometry: '%s'", arg);
		}

		/* Additional data file */
		else if(!strcmp(arg, "--load"))
		{
			arg = argv[++i];
			if(!arg) error("expected argument after --load");
			else if(read_load(arg, opt))
				error("invalid load argument: '%s'", arg);
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
"usage: %s [-r|-d] [-g] <binary> [options...]\n\n"

"Available run modes (default is -r):\n"
"  -r | --run         Normal execution, shows CPU state when program ends\n"
"  -d | --debug       Run the debugger: step-by-step, breakpoints, etc\n"
"  -g | --graphical   With -r or -d, enable graphical I/O using SDL\n\n"

"Available options:\n"
"  --geometry <text>:<stack>:<data>:<vram>\n"
"                     Set the size of the four memory segments ('k' or 'M'\n"
"                     suffixes may be used)\n";

/* Emulated memory and CPU */
static memory_t *mem	= NULL;
static cpu_t *cpu	= NULL;

/*
	help()
	Prints the help message and exits successfully.

	@arg	argv	Argument array; argv[0] is used for invocation help
*/
void help(const char **argv)
{
	printf(help_string, argv[0]);
	exit(0);
}

/*
	quit()
	Exit handler. Destroys what was left behind by the main function.
*/
void quit(void)
{
	if(cpu) cpu_destroy(cpu);
	if(mem) memory_destroy(mem);
}

/*
	main()
	In a normal execution flow, parses the command-line arguments, creates
	a virtual CPU and memory, loads the provided file into memory, then
	starts the debugger and give it control of the flow.

	@arg	argc	Number of command-line arguments
	@arg	argv	NULL-terminated command-line argument array
	@returns	Status code: 0 on success, 1 on error
*/
int main(int argc, char **argv)
{
	atexit(quit);

	/* Parse command-line arguments */
	opt_t opt;
	parse_args(argc, argv, &opt);

	/* Check that a filename was provided */
	if(argc == 1 || opt.help) help((const char **)argv);
	if(!opt.filename) fatal("no input file");

	/* Allocate a virtual memory and load the program into it */
	mem = memory_new(opt.text, opt.stack, opt.data, opt.vram);
	memory_load_program(mem, opt.filename);

	/* Load an additional file if requested */
	if(opt.load_file)
	{
		int x = memory_load_file(mem, opt.load_addr, opt.load_file);
		if(x) return 1;
	}

	/* Create a CPU and give it the memory */
	cpu = cpu_new(mem);

	/* It's show time! */

	if(opt.graphical)
	{
		void *vram = (void *)mem->mem + (mem->vram >> 3);
		if(graphical_start(160, 128, vram)) return 1;
	}

	if(!opt.debugger)
	{
		while(cpu->ptr[PC] < mem->text && !cpu->h)
			cpu_execute(cpu);

		puts("At end of execution:");
		cpu_dump(cpu, stdout);

		/* In run mode, the execution may stop very quickly; leave the
		   window open until the user closes it */
		if(opt.graphical)
		{
			graphical_freeze();
			puts("\nThe program will exit when you close the "
				"graphical window.");
			graphical_wait();
		}
	}
	else
	{
		debugger(opt.filename, cpu);

		/* By contrast, the debugger mode can only end when the user
		   explicity requires it, so close the window immediately */
		graphical_stop();
	}

	return 0;
}
