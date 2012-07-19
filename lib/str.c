/**
 *@file str.c
 *@brief various string-related functions
 *@athor Slavomir Vlcek
 *@copyright GPLv2
 */

#include <stdbool.h>

#include "debug.h"


/**
 *@brief Reverses given sequence of characters (is not '\0' controlled)
 *
 *@param seq
 *@param len
 */
void _rev_char_seq(char *const seq, const int len)
{
        int head = 0;
        int tail = len - 1;

        /* auxiliary var */
        char aux;

        while(head < tail)
        {
                aux = seq[head];

                seq[head] = seq[tail];

                seq[tail] = aux;

                ++head;
                --tail;
        }
}


/**
 *@brief Determines if next variable would get out of array range
 */
bool _position_is_out_of_range(const char *curr_pos, const char *limit,
				      const int size)
{

	/* if position is out of array range */
	return curr_pos + size >= limit;
}


/**
 *@brief Get length of string excluding '\0' char
 *
 *@param start point which we should count chars from
 *@param limit end of array that cannot be crossed
 *
 *@return length or -1 if stream end comes
 */
int _safe_str_len(const char *start, const char *limit)
{
	int cnt = 0;
	/* iterate until '\0' is found */
	while(*start) {

		GUARD(_position_is_out_of_range(start, limit, 1), -1);
		++cnt;
		/* shift pos pointer */
		++start;
	}
	return cnt;
}

