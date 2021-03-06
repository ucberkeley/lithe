/*
 * Basic perror + exit routines.
 *
 * Contributed by Benjamin Hindman <benh@berkeley.edu>, 2008.
 */

#ifndef FATAL_H
#define FATAL_H

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#ifndef DEBUG

/*
 * Prints the string specified by 'fmt' to standard error before
 * calling exit.
 */
void fatal(char *fmt, ...);

/*
 * Prints the string specified by 'fmt' to standard error along with a
 * string containing the error number as specfied in 'errno' before
 * calling exit.
 */
void fatalerror(char *fmt, ...);

#else

/*
 * Like the non-debug version except includes the file name and line
 * number in the output.
 */
#define fatal(fmt...) __fatal(__FILE__, __LINE__, fmt)
void __fatal(char *file, int line, char *fmt, ...);

/*
 * Like the non-debug version except includes the file name and line
 * number in the output.
 */
#define fatalerror(fmt...) __fatalerror(__FILE__, __LINE__, fmt)
void __fatalerror(char *file, int line, char *fmt, ...);

#endif // DEBUG

#endif // FATAL_H
