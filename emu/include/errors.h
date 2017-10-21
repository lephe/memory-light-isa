#ifndef	ERROR_H
#define	ERROR_H

/* error_t -- error types and behaviors */
typedef enum
{
	error_note	= 0,	/* Output contents to stderr */
	error_warn	= 1,	/* Warn and continue execution */
	error_error	= 2,	/* Print error and continue execution */
	error_ierror	= 3,	/* Display an internal error and continue */
	error_fatal	= 4,	/* Display a fatal error and exit(1); */
	error_ifatal	= 5,	/* Display an internal error and exit(1); */
} error_t;

/* error_msg() -- emit an error
   If format starts with "# ", prints strerror(errno) after the message. */
void error_msg(error_t level, const char *format, ...);

/* error_clear() -- clear the error flag
   The error flag is set every time error or internal errors are emitted. */
void error_clear(void);

/* error_check() -- check the error flag
   If the error flag is set, exits the program. */
void error_check(void);

/* Some useful wrappers */
#define	note(format, ...)	error_msg(error_note,	format, ##__VA_ARGS__)
#define	warn(format, ...)	error_msg(error_warn,	format, ##__VA_ARGS__)
#define	error(format, ...)	error_msg(error_error,	format, ##__VA_ARGS__)
#define	ierror(format, ...)	error_msg(error_ierror,	format, ##__VA_ARGS__)
#define	fatal(format, ...)	error_msg(error_fatal,	format, ##__VA_ARGS__)
#define	ifatal(format, ...)	error_msg(error_ifatal,	format, ##__VA_ARGS__)

#endif	/* ERROR_H */
