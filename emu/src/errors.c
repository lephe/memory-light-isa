#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>

#include <errors.h>

/* This flag allows the user to delay the program termination on error, taking
   time to check more input and detect more errors */
static int error_flag = 0;

/* error_msg() -- emit an error */
void error_msg(error_t level, const char *format, ...)
{
	/* Heed for perror() calls */
	int call_perror = 0, e = errno;
	if(format[0] == '#')
	{
		format++;
		if(format[0] == ' ') call_perror = 1, format++;
	}

	/* Indicate the type of message */
	if(level > 0 && level <= 5)
	{
		const char *strs[] = {
			"note: ", "warning: ", "error: ", "internal error: ",
			"fatal error: ", "internal error: ",
		};
		int colors[] = { 37, 33, 31, 35, 31, 35 };

		fprintf(stderr, "\e[%d;1m%s\e[0m", colors[level], strs[level]);
	}

	/* Print the given message */
	va_list args;
	va_start(args, format);
	vfprintf(stderr, format, args);
	va_end(args);

	/* If required, call strerror() for more detail */
	if(call_perror) fputs(": ", stderr), fputs(strerror(e), stderr);
	putchar('\n');

	/* Set the error flag or leave when required */
	if(level >= error_error) error_flag = 1;
	if(level >= error_fatal) exit(1);
}

/* error_clear() -- clear the error flag
   The error flag is set every time error or internal errors are emitted. */
void error_clear(void)
{
	error_flag = 0;
}

/* error_check() -- check the error flag
   If the error flag is set, exits the program. */
void error_check(void)
{
	if(error_flag) exit(1);
}
