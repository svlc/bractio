/**
 * @file check.c
 * @brief 
 * @athor Slavomir Vlcek
 * @copyright GPLv2
 */

#include <string.h>

#include "rapm.h"
#include "debug.h"

#include "check.h"


/**
 * @brief Check if there is no 0-bit with lower significancy than "MS1-bit".
 * @note Also detects if all 1-bits correspond to valid tasks (@see task_t).
 * 
 * @param task
 * @return 0 on success, 1 on failure
 */
int check_task(task_t task)
{
#define GREATEST_TASK_BIT 2

	task_t mask = 1;
	task_t old_mask = 1;
	unsigned bit_sig = 0;

	/* while there are just 1-bits */
	while (mask == (mask & task)) {

		if (bit_sig > GREATEST_TASK_BIT) {
			return 1;
		}
		/* back up mask */
		old_mask = mask;
		mask <<= 1;
		/* change LSB to "1" */
		mask |= 1;
		++bit_sig;
	}

	if (old_mask != task) {
		/* there is some remaining 1-bit to the left */
		return 1;
	}

	return 0;

#undef GREATEST_TASK_BIT
}


/**
 * @brief 
 */
static int range_magic_id(const char *magic_id)
{
	char *str = "Warcraft III recorded game\32";

	if (0 != memcmp(magic_id, str, sizeof(str) + 1)) {
		return 1;
	}
	return 0;
}


/**
 * @brief 
 */
static int range_strm_offset(const unsigned sub_hdr_ver, const unsigned strm_offset)
{

	if (sub_hdr_ver == 0  &&  strm_offset != 0x40) {
		return 1;
	} else if (strm_offset != 0x44) {
		return 1;
	}
	return 0;
}


/**
 * @brief 
 */
static int range_sub_hdr_ver(const unsigned sub_hdr_ver)
{
	if (sub_hdr_ver >= 2) {
		return 1;
	}
	return 0;
}


/**
 * @brief 
 */
int check_rep_main_hdr(char *magic_id, main_hdr_t *m_hdr)
{
	GUARD(0 != range_magic_id(magic_id), return APM_E_RANGE);

	GUARD(0 != range_sub_hdr_ver(m_hdr->sub_hdr_ver), return APM_E_RANGE);

	/*
	 * this is done later than sub_hdr_ver because
	 * we need to pass variable "sub_hdr_ver" already checked
	 */
	GUARD(0 != range_strm_offset(m_hdr->sub_hdr_ver, m_hdr->strm_offset),
		return APM_E_RANGE);

	return 0;
}


/**
 * @brief
 */
static int range_rls(char *rls_seq)
{
#define RLS_SEQ_LEN 4

	if (0 != memcmp("3RAW", rls_seq, RLS_SEQ_LEN)
	    && 0 != memcmp("PX3W", rls_seq, RLS_SEQ_LEN)) {
		return 1;
	}
	return 0;

#undef RLS_SEQ_LEN
}


/**
 * @brief 
 */
static int range_patch_ver(unsigned patch_ver)
{
	/* this patch version is very unlikely to be ever released */
	if (patch_ver > 40) {
		return 1;
	}
	return 0;
}


/**
 * @brief 
 */
static int range_player_mode(unsigned player_mode)
{
	if (player_mode != 0x0	&&  player_mode != 0x8000) {
		return 1;
	}
	return 0;
}


/**
 * @brief
 *
 * @param rls_seq release sequence, must have 4 chars or be NULL
 */
int check_rep_sub_hdr(sub_hdr_t *s_hdr, char *rls_seq)
{
	if (rls_seq) {
		GUARD(range_rls(rls_seq), return APM_E_RANGE);
	}
	GUARD(0 != range_patch_ver(s_hdr->patch_ver), return APM_E_RANGE);
	GUARD(0 != range_player_mode(s_hdr->player_mode), return APM_E_RANGE);

	return 0;
}


int check_action_change_selection(unsigned mode)
{
	if (mode != 0x01  &&  mode != 0x02) {
		return APM_E_RANGE;
	}
	return 0;
}
