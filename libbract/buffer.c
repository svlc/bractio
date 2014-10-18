/**
 * @file buffer.h
 * @brief Char buffer
 * @athor Slavomir Vlcek
 * @copyright GPLv2
 */

#include <stdlib.h>		/* NULL, {m,c}alloc(3), free(3) */
#include <errno.h>		/* errno */
#include <stdbool.h>
#include <assert.h>
#include <stdio.h>

#include "buffer.h"

/**
 * @brief Zero all buffer variables.
 * 
 * @param buff
 */
static void buff_zero(buff_t *buff)
{
	buff->arr = NULL;
	buff->pos = NULL;
	buff->len = 0;
	buff->lim = NULL;
}

/**
 * @brief Prepare all resources.
 * 
 * @param buff
 * @param len wanted length of buffer
 */
int buff_prep(buff_t *buff, const size_t len)
{
	buff_zero(buff);

	buff->arr = (char *)malloc(len);
	if (NULL == buff->arr) {
		return 1;
	}

	buff_pos_reset(buff);
	buff->len = len;

	/* set "limit" to first 'out of range' char */
	buff->lim = buff->arr + len;

	return 0;
}

/**
 * @brief Free all resources, zero buffer.
 * 
 * @param buff
 */
void buff_empty(buff_t *buff)
{
	free(buff->arr);
	buff_zero(buff);
}

/**
 * @brief Set position to the beginning of buffer array.
 * 
 * @param buff
 */
void buff_pos_reset(buff_t *buff)
{
	buff->pos = buff->arr;
}
