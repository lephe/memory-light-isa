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
	/* Heed for strerror() calls. Save errno now because something could
	   happen inside fprintf() or any other function */
	int call_strerror = 0;
	int e = errno;

	if(format[0] == '#')
	{
		format++;
		if(format[0] == ' ') call_strerror = 1, format++;
	}

	/* Indicate the type of message with a bit of coloring */
	if(level >= 0 && level <= 5)
	{
		const char *strs[] = {
			"note: ", "warning: ", "error: ", "internal error: ",
			"fatal error: ", "internal error: ",
		};
		int colors[] = { 37, 33, 31, 35, 31, 35 };

		fprintf(stderr, "\e[%d;1m%s\e[0m", colors[level], strs[level]);
	}

	/* Don't forget to actually print the provided message */
	va_list args;
	va_start(args, format);
	vfprintf(stderr, format, args);
	va_end(args);

	/* If requested, call strerror() for more detail */
	if(call_strerror) fputs(": ", stderr), fputs(strerror(e), stderr);
	putchar('\n');

	/* Set the error flag or leave when required */
	if(level >= error_error) error_flag = 1;
	if(level >= error_fatal) exit(1);
}

/* error_clear() -- clear the error flag */
void error_clear(void)
{
	error_flag = 0;
}

/* error_check() -- check the error flag */
void error_check(void)
{
	if(error_flag) exit(1);
}
