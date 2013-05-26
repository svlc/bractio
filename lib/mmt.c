/**
 * @file mmt.c
 * @brief
 * @athor Slavomir Vlcek
 * @copyright GPLv2
 */

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "mmt.h"

typedef struct unit_t {
	unsigned *ptr;
	unsigned max;
} unit_t;

/**
 * @brief Zero mmt_t variables.
 *
 * @param mmt
 */
void moment_zero(mmt_t *mmt)
{
	memset(mmt, 0, sizeof(*mmt));
}

/**
 * @brief Get moment in minutes.
 *
 * @param mmt
 * @return number of minutes
 */
double moment_min(mmt_t *mmt)
{
#define MS_IN_MINUTE 60000.0

	return (double)mmt->total_ms / MS_IN_MINUTE;

#undef MS_IN_MINUTE
}

/**
 * @brief Increment total_time and update hour/min/sec/ms accordingly.
 *
 * @note while condition "cnt < 4" may be dispensable
 */
void moment_inc(mmt_t *mmt, unsigned long time_inc)
{
	/* auxiliary array */
	unit_t unit_arr[4] = {
		{ &mmt->ms, 1000 },
		{ &mmt->sec, 60 },
		{ &mmt->min, 60 },
		{ &mmt->hour, 1000000 }
	};

	div_t dd;
	size_t idx = 0;
	unsigned long cnt = time_inc;

	mmt->total_ms += time_inc;

	do {
		cnt += *unit_arr[idx].ptr;
		
		if (cnt >= unit_arr[idx].max) {
			dd = div(cnt, unit_arr[idx].max);
			*unit_arr[idx].ptr = dd.rem;
			cnt = dd.quot;
		} else {
			*unit_arr[idx].ptr = cnt;
			/* meaningless to continue */
			break;
		}
		++idx;
	} while (dd.quot && idx < 4);
}
