//---
//	emu:errors -- top-level error management
//
//	This module provides facilities for reporting errors at every point of
//	the program where the ncurses interface is not active. These facilities
//	consist of a generic message-printing routine with automatic behavior
//	and an "error flag".
//
//	The error flag can be cleared manually, and is set every time an error
//	of a level at least that of error_error (see error_t enumeration) is
//	emitted. When processing a chunk of data, one might want to continue
//	processing even on error, to detect further problems. The error flag
//	remembers errors so that the procedure can fail at the end of the
//	process, if anything went wrong.
//---

#ifndef	ERROR_H
#define	ERROR_H

/*
	error_t enumeration
	This type enumerates the error types supported by the module. Each of
	them is associated with a specific component (and sometimes a specific
	message such as "error:" displayed automatically).
*/
typedef enum
{
	error_note	= 0,	/* Output contents to stderr */
	error_warn	= 1,	/* Warn and continue execution */
	error_error	= 2,	/* Print error and continue execution */
	error_ierror	= 3,	/* Display an internal error and continue */
	error_fatal	= 4,	/* Display a fatal error and exit(1); */
	error_ifatal	= 5,	/* Display an internal error and exit(1); */
} error_t;

/*
	error_msg() -- emit an error
	This function displays the given message, provided through a printf()-
	like format and variable arguments, prefixed with a small string
	indicating the kind of error (such as "warning:").
	If format starts with "# " (a sharp followed by a space), this function
	also prints strerror(errno) after the message. The sharp sign may be
	escaped by writing "##".

	@arg	level	The kind of error
	@arg	format	printf()-like format for the message
	@arg	...	Arguments required by the format
*/
void error_msg(error_t level, const char *format, ...);

/*
	error_clear() -- clear the error flag
	The error flag is only cleared manually, but is set automatically
	every time an error or internal error is emitted.
*/
void error_clear(void);

/*
	error_check() -- check the error flag
	Asserts that the error flag is clear. If it's set, calls exit(1).
*/
void error_check(void);

/* And here are some useful wrappers, basically the ones you will want to be
   using instead of error_msg() */
#define	note(format, ...)	error_msg(error_note,	format, ##__VA_ARGS__)
#define	warn(format, ...)	error_msg(error_warn,	format, ##__VA_ARGS__)
#define	error(format, ...)	error_msg(error_error,	format, ##__VA_ARGS__)
#define	ierror(format, ...)	error_msg(error_ierror,	format, ##__VA_ARGS__)
#define	fatal(format, ...)	error_msg(error_fatal,	format, ##__VA_ARGS__)
#define	ifatal(format, ...)	error_msg(error_ifatal,	format, ##__VA_ARGS__)

#endif	/* ERROR_H */
