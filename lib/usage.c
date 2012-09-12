/**
 * @file usage.c
 * @brief
 * @athor Slavomir Vlcek
 * @copyright GPLv2
 */

#include <stdio.h>
#include <stdarg.h>

/**
 * @brief Report error to stderr.
 *
 * @param fmt format string 
 */
void rep_err(char *fmt, ...)
{
#define STR_LEN 300

	va_list ap;
	char str[STR_LEN];

	va_start(ap, fmt);

	vsnprintf(str, STR_LEN, fmt, ap);
	va_end(ap);

	/* vsnprintf always adds '\0' */
	fprintf(stderr, str);

#undef STR_LEN
}

