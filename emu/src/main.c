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

/*
	opt_t structure
	Describes the command-line parameters passed to the program.
*/
typedef struct
{
	/* TODO - Think more about how these should work (and compatibility
	   with the subject's model) */
	enum {
		mode_run	= 1,
		mode_debug	= 2,
		mode_graphical	= 3,
	} mode		:8;
	uint help	:1;

	uint64_t	memsize;	/* Dynamic memory size */
	uint64_t	stack;		/* Dynamic bottom stack address */

	const char *filename;		/* Emulated binary file */
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
	parse_args() -- parse command-line options into an opt_t object

	@arg	argc	Number of command-line arguments
	@arg	argv	NULL-terminated command-line argument array
	@arg	opt	Option structure to fill with the argument content
*/
static void parse_args(int argc, char **argv, opt_t *opt)
{
	/* Clear structure with default values */
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

		/* Reject mode arguments if finding more than one */
		if(newmode)
		{
			if(opt->mode) error("unexpected mode: '%s'", arg);
			else opt->mode = newmode;
		}

		/* Arguments related to memory geometry */
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

	if(opt.mode == mode_run)
		fatal("cannot honor run mode x_x (TODO)");
	if(opt.mode == mode_graphical)
		fatal("cannot honor graphical mode x_x (TODO)");

	/* Allocate a virtual memory and load the program into it */
	mem = memory_new(opt.memsize, opt.stack, 0);
	memory_load(mem, opt.filename);

	/* Create a CPU and give it the memory */
	cpu = cpu_new(mem);

	/* Start the debugger. It's show time! */
	debugger(opt.filename, cpu);

	return 0;
}
