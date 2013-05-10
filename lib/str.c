/**
 * @file str.c
 * @brief various string-related functions
 * @athor Slavomir Vlcek
 * @copyright GPLv2
 */


#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <inttypes.h>

#include "str.h"

#include "rapm.h"
#include "debug.h"


/**
 * @brief Determine if position equals limit.
 * @note Fact that (pos == lim) means that pos points to first item behind
 * array end. It signals that every stream character was read.
 *
 * @param pos 
 * @param lim 
 * @return 
 */
bool pos_eq_lim(const char *pos, const char *lim)
{
	return pos == lim;
}


/**
 * @brief Determine if position is greater than limit.
 *
 * @param pos position to be controlled
 * @param lim first char behind array range
 * @return 
 */
static inline bool pos_gt_lim(const char *pos, const char *lim)
{
	/* if position is out of array range */
	return pos > lim;
}


/**
 * @brief Get length of string excluding '\0' char.
 *
 * @param start point which we should count chars from
 * @param lim
 *
 * @return length or -1 in case of stream end
 */
static int safe_str_len(const char *start, const char *lim)
{
	unsigned cnt = 0;

	while (1) {
		if (pos_gt_lim(start + 1, lim)) {
			cnt = -1;
			break;
		}
		/* iterate until '\0' is found */
		if (0 == *start) {
			break;
		}
		++cnt;
		/* shift local position */
		++start;
	}
	return cnt;
}


/**
 * @brief Safe position shift forward.
 * @note We do not want this function to act like fread(3). Either pos
 * can be shifted, or it would get out of array range.
 *
 * @param pos
 * @param lim first item behind array
 * @param size
 * @return 0 on success
 */
int safe_pos_fw(char **pos, char *lim, const size_t size)
{
	*pos += size;

	/* if position is out of array range */
	if (pos_gt_lim(*pos, lim)) {
		return 1;
	}
	return 0;
}


/**
 * @brief Unsafe position shift backward.
 *
 * @param pos 
 * @param size
 */
void unsafe_pos_bw(char **pos, const size_t size)
{
	*pos -= size;
}


/**
 * @brief Possibly inspect item size (if size is 0), also check array limits.
 *
 * @param start beginning char of item
 * @param lim first out-of-range item
 * @param item single aux_t item
 * @return 0 on success
 */
static int inspect_item_size(char *start, const char *lim, aux_t *item)
{
	int ret;

	/*
	 * if size of item is "0", string is expected
	 * so we have to compute the length at first
	 */
	if (0 == item->size) {
		ret = safe_str_len(start, lim);
		if (-1 == ret) {
			ret = 1;
			goto out;
		}
		*((char **)item->dest) = (char *)malloc(ret + 1);
		if (!*((char **)item->dest)) {
			ret = APM_E_NO_MEM;
			goto out;
		}
		item->dest = *((char **)item->dest);
		/* now we know the size of string */
		item->size = ret + 1;
	} else {
		if (pos_gt_lim(start + item->size, lim)) {
			ret = 1;
			goto out;
		}
	}
	ret = 0;
out:
	return ret;
}


/**
 * @brief
 * @note This is the only function that uses fixed-width integer types.
 *
 * @param pos current position
 * @param lim first out-of-range char
 * @param aux_arr
 * @param cnt number of aux_t items
 * @return 0 on success
 */
int safe_mem_read(char **pos, const char *lim, aux_t *aux_arr, const size_t cnt)
{
	int ret;

	for (size_t idx = 0;  idx < cnt;  ++idx) {

		ret = inspect_item_size(*pos, lim, &aux_arr[idx]);
		if (0 != ret) {
			return ret;
		}
		if (APM_UCHAR == aux_arr[idx].type) {
			memcpy(aux_arr[idx].dest, *pos, aux_arr[idx].size);
		}
		else if (APM_UINT == aux_arr[idx].type) {
			if (aux_arr[idx].size == 4) {
				*(unsigned *)aux_arr[idx].dest
					= **(uint32_t **)pos;
			} else if (aux_arr[idx].size == 2) {
				*(unsigned *)aux_arr[idx].dest
					= **(uint16_t **)pos;
			} else if (aux_arr[idx].size == 1) {
				*(unsigned *)aux_arr[idx].dest
					= **(uint8_t **)pos;
			}
		}
		else if (APM_ULONG == aux_arr[idx].type) {

			if (4 == aux_arr[idx].size) {
				*(unsigned long *)aux_arr[idx].dest
					= **(uint32_t **)pos;
			} else if (2 == aux_arr[idx].size) {
				*(unsigned long *)aux_arr[idx].dest
					= **(uint16_t **)pos;
			}
		}

		/* shift buffer offset */
		*pos += aux_arr[idx].size;
	}
	return 0;
}
