#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdint.h>

#include <unistd.h>
#include <signal.h>

#include <defs.h>
#include <errors.h>
#include <memory.h>
#include <cpu.h>
#include <debugger.h>
#include <graphical.h>

/* I wished to have limited the use of SDL to graphical.h, but I need it here
   to map some keyboard keys to Chip8's 16 keys */
#include <SDL2/SDL.h>

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
	uint chip8	:1;

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
	*opt = (opt_t){ 0 };

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

		/* Special handlers for the Chip8 emulator */
		else if(!strcmp(arg, "--chip8")) opt->chip8 = 1;

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
"  -r | --run	 Normal execution, shows CPU state when program ends\n"
"  -d | --debug       Run the debugger: step-by-step, breakpoints, etc\n"
"  -g | --graphical   With -r or -d, enable graphical I/O using SDL\n\n"

"Available options:\n"
"  --geometry <text>:<stack>:<data>:<vram>\n"
"		     Set the size of the four memory segments ('k' or 'M'\n"
"		     suffixes may be used)\n";

/* Emulated memory and CPU */
static memory_t *mem	= NULL;
static cpu_t *cpu	= NULL;
/* PID of the main thread */
pid_t main_thread	= 0;

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
	sigh()
	Signal handler for graceful program termination on crash.

	@arg	signum	Identifier of received signal.
*/
void sigh(int signum)
{
	size_t *counts = cpu_counts();

	for(uint i = 0; i < DISASM_INS_COUNT; i++)
	{
		const char *format = disasm_format(i);
		printf("  %6s %-6zu", format + 6, counts[i]);
		if((i & 7) == 7) printf("\n");
	}

	if((DISASM_INS_COUNT & 7) != 7) printf("\n");

	/* Prevent the terminal from getting screwed up */
	endwin();
	/* Free the resources used by the emulator, as in normal termination */
	quit();

	/* Since we override the default handler, we need to provide
	   information on what went wrong by ourselves */
	if(signum == SIGINT)
		write(STDERR_FILENO, "Interrupted.\n", 13);
	else if(signum == SIGSEGV)
		write(STDERR_FILENO, "Segmentation fault!\n", 20);
	else if(signum == SIGTERM)
		write(STDERR_FILENO, "Terminated.\n", 12);
	else
	{
		char str[] = "Killed by signal <>.\n";
		str[17] = '0' + signum / 10;
		str[18] = '0' + signum % 10;
		write(STDERR_FILENO, str, 20);
	}

	/* exit() is not async-signal-safe because of exit handlers, however
	   _exit() is */
	_exit(2);
}

/*
	sigh_sleep()
	Signal handler for sleep events.

	@arg	signum	Signal id (always SIGUSR1)
*/
void sigh_sleep(__attribute__((unused)) int sigusr1)
{
	cpu->sleep = 0;
}

/*
	chip8()
	Emulator callback that maintains counters and keyboard state in the
	chip8 emulator's memory. This function is controlled by the --chip8
	switch.
	This function is not executed from the main thread! This must be the
	only thread that accesses the timer counters in write mode, and it must
	do it atomically, to prevent data races.
*/
void chip8(const uint8_t *keyboard, void *arg)
{
	cpu_t *cpu = arg;
	uint64_t timer_delay	= 0x881e0, delay;
	uint64_t timer_audio	= 0x881f0, audio;
	uint64_t timer_cpufreq	= 0x88240, cpufreq;

	delay = memory_read(cpu->mem, timer_delay, 8);
	if(delay) memory_write(cpu->mem, timer_delay, delay - 1, 8);

	audio = memory_read(cpu->mem, timer_audio, 8);
	if(audio) memory_write(cpu->mem, timer_audio, audio - 1, 8);

	cpufreq = memory_read(cpu->mem, timer_cpufreq, 8);
	memory_write(cpu->mem, timer_cpufreq, cpufreq + 10, 8);

	uint64_t keybuffer	= 0x881b0;

	#define _(x) SDL_SCANCODE_ ## x
	SDL_Scancode keys[16] = {
		_(1), _(2), _(3), _(4),
		_(Q), _(W), _(E), _(R),
		_(A), _(S), _(D), _(F),
		_(Z), _(X), _(C), _(V),
	};
	#undef _

	uint16_t state = 0;
	for(int i = 0; i < 16; i++)
	{
		state <<= 1;
		if(keyboard[keys[i]]) state |= 1;
	}

	memory_write(cpu->mem, keybuffer, state, 16);

	/* Wake up the sleeping processor */
	kill(main_thread, SIGUSR1);
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
	/* Register the exit handler for normal program termination */
	atexit(quit);

	main_thread = getpid();

	/* Install some signal handlers in case the emulator crashes or is
	   interrupted by the user */
	struct sigaction action = { 0 };
	action.sa_handler	= sigh;
	action.sa_flags		= SA_RESTART;
	sigemptyset(&action.sa_mask);

	/* SIGINT: Interrupted by Ctrl+C */
	sigaction(SIGINT,  &action, NULL);
	/* SIGSEGV: Segmentation faults */
	sigaction(SIGSEGV, &action, NULL);
	/* SIGTERM: Killed by user/system (often) */
	sigaction(SIGTERM, &action, NULL);

	/* And some other handler to handle deep sleep */
	action.sa_handler	= sigh_sleep;
	action.sa_flags		= SA_RESTART;
	sigemptyset(&action.sa_mask);

	/* SIGUSR1: Waken by timer */
	int x = sigaction(SIGUSR1, &action, NULL);
	printf("%d\n", x);

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
		int result = !opt.chip8
			? graphical_start(160, 128, vram, NULL, NULL, 2)
			: graphical_start(64, 32, vram, chip8, cpu, 4);
		if(result) return 1;
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
		   explicitly requires it, so close the window immediately */
		graphical_stop();
	}

	return 0;
}
