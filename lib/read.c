/**
 * @file read.c
 * @brief Pile of function that saves replay data into memory.
 * @athor Slavomir Vlcek
 * @copyright GPLv2
 */

#include <stdio.h>		/* fread(3) */
#include <stdlib.h>		/* NULL */
#include <string.h>		/* memcpy(3), strerror(3) */
#include <inttypes.h>		/* fixed width integer types */
#include <errno.h>
#include <stdbool.h>
#include <assert.h>


#include "rapm.h"
#include "debug.h"
#include "init_deinit.h"
#include "decode.h"
#include "check.h"
#include "str.h"
#include "stream.h"
#include "usage.h"

#include "read.h"


/**
 * @brief Save sub header (v0) data from file into memory.
 */
static int read_rep_sub_hdr_v0(sub_hdr_t *sub_hdr, buff_t *buff, FILE *fp)
{
#define SUB_HDR_MEMB_CNT  6

	assert(buff->len >= 0x10);

	int ret;

	buff_pos_reset(buff);

	ret = fread(buff->pos, 1, 0x10, fp);
	VGUARD(0x10 != ret, rep_err("Just %i byte(s) of sub header were read.",
				    ret); return APM_E_FILE_READING);

	/* v0 has different size of "patch_ver" */
	unsigned patch_ver;

	/* this variable will not be saved */
	unsigned unknown;
	
	aux_t aux_arr[SUB_HDR_MEMB_CNT] = {

		{ &unknown, APM_UINT, 2 },
		{ &patch_ver, APM_UINT, 2 },
		{ &sub_hdr->build, APM_UINT, 2 },
		{ &sub_hdr->player_mode, APM_UINT, 2 },
		{ &sub_hdr->rep_len_ms, APM_ULONG, 4 },
		{ &sub_hdr->hdr_CRC32, APM_ULONG, 4 }
	};

	ret = safe_mem_read(&buff->pos, buff->lim, aux_arr, SUB_HDR_MEMB_CNT);
	if (0 != ret) {
		return ret;
	}

	ret = check_rep_sub_hdr(sub_hdr, NULL);
	if (0 != ret) {
		return ret;
	}

	return 0;
}

/**
 * @brief Save sub header (v1) data from file into memory.
 */
static int read_rep_sub_hdr_v1(sub_hdr_t *sub_hdr, buff_t *buff, FILE *fp)
{
#define RLS_SEQ_LEN 4
	assert(buff->len >= 0x14);

	int ret;
	char rls_seq[4];

	buff_pos_reset(buff);
	/* read 0x14 bytes */
	ret = fread(buff->pos, 1, 0x14, fp);
	VGUARD(0x14 != ret, rep_err("Just %i byte(s) of sub header were read.",
				    ret); return APM_E_FILE_READING);

	/* auxiliary struct array */
	aux_t aux_arr[SUB_HDR_MEMB_CNT] = {

		{ rls_seq, APM_UCHAR, 4 },
		{ &sub_hdr->patch_ver, APM_UINT, 4 },
		{ &sub_hdr->build, APM_UINT, 2 },
		{ &sub_hdr->player_mode, APM_UINT, 2 },
		{ &sub_hdr->rep_len_ms, APM_ULONG, 4 },
		{ &sub_hdr->hdr_CRC32, APM_ULONG, 4 }
	};

	ret = safe_mem_read(&buff->pos, buff->lim, aux_arr, SUB_HDR_MEMB_CNT);
	if (0 != ret) {
		return ret;
	}

	ret = check_rep_sub_hdr(sub_hdr, rls_seq);
	if (0 != ret) {
		return ret;
	}

	if (0 == memcmp("WAR3", rls_seq, RLS_SEQ_LEN)) {
		sub_hdr->rls = 0;
	}
	/* else it must be "W3XP" */
	else {
		sub_hdr->rls = 1;
	}

	return 0;
#undef RLS_SEQ_LEN
#undef SUB_HDR_MEMB_CNT
}

/**
 * @brief Save main header data from file into memory.
 *
 * @param m_hdr main header
 * @param buff character buffer
 * @param fp file pointer
 * @return 0 on success
 */
int read_rep_main_hdr(main_hdr_t *m_hdr, buff_t *buff, FILE *fp)
{
#define MAIN_HDR_MEMB_CNT 6

	assert(buff->len >= 0x30);

	int ret;
	/* reserved space for '0' */
	char magic_id[28];

	buff_pos_reset(buff);
	/* read 0x30 bytes */
	ret = fread(buff->pos, 1, 0x30, fp);
	VGUARD(0x30 != ret, rep_err("Just %i byte(s) of main header were read.",
				    ret); return APM_E_FILE_READING);

	/* auxiliary struct array */
	aux_t aux_arr[MAIN_HDR_MEMB_CNT] = {

		{  magic_id,			APM_UCHAR,	28 },
		{ &m_hdr->strm_offset,		APM_UINT,	4  },
		{ &m_hdr->total_file_size,	APM_UINT,	4  },
		{ &m_hdr->sub_hdr_ver,		APM_UINT,	4  },
		{ &m_hdr->dcd_data_size,	APM_UINT,	4  },
		{ &m_hdr->ecd_sgmt_cnt,		APM_UINT,	4  }
	};

	ret = safe_mem_read(&buff->pos, buff->lim, aux_arr, MAIN_HDR_MEMB_CNT);
	if (0 != ret) {
		return ret;
	}

	ret = check_rep_main_hdr(magic_id, m_hdr);
	if (0 != ret) {
		return ret;
	}

	return 0;
#undef MAIN_HDR_MEMB_CNT
}

/**
 * @brief Read main header and subheader.
 *
 * @param
 * @param
 * @param
 * @param
 *
 * @return 0 on success
 */
int read_rep_hdr(main_hdr_t *main_hdr, sub_hdr_t *sub_hdr,
		 buff_t *buff, FILE *fp)
{
	int ret;

	ret = read_rep_main_hdr(main_hdr, buff, fp);
	if (0 != ret) {
		return ret;
	}

	if (0x00 == main_hdr->sub_hdr_ver) {
		ret = read_rep_sub_hdr_v0(sub_hdr, buff, fp);
		if (0 != ret) {
			return ret;
		}
	}
	else if (0x01 == main_hdr->sub_hdr_ver) {
		ret = read_rep_sub_hdr_v1(sub_hdr, buff, fp);
		if (0 != ret) {
			return ret;
		}
	}
	/*
	 * else branch is meaningless -- sub_hdr_ver was checked,
	 * @see read_rep_main_hdr()
	 */
	return 0;
}
	
/**
 * @brief Save encoded "segment" body from file.
 */
int read_sgmt_body(sgmt_t *sgmt, FILE *fp)
{
	size_t ret;

	/* save encoded chunk from file into memory */
	ret = fread(sgmt->ecd_data, 1, sgmt->ecd_size, fp);
	if (ret != sgmt->ecd_size) {
		return APM_E_FILE_READING;
	}

	return 0;
}

/**
 * @brief Save "segment" header from file to memory.
 *
 * @param sgmt empty segment
 * @param
 * @param
 * @return 
 */
int read_sgmt_hdr(sgmt_t *sgmt, buff_t *buff, FILE *fp)
{
#define SGMT_MEMB_CNT	3

	assert(buff->len >= 0x08);

	int ret;

	buff_pos_reset(buff);

	if (0x08 != fread(buff->pos, 1, 0x08, fp)) {
		return 1;
	}


	aux_t aux_arr[SGMT_MEMB_CNT] = {

		{ &sgmt->ecd_size,	APM_UINT, 2 },
		{ &sgmt->dcd_size,	APM_UINT, 2 },
		{ &sgmt->unknown,	APM_UINT, 4 }
	};

	ret = safe_mem_read(&buff->pos, buff->lim, aux_arr, SGMT_MEMB_CNT);
	if (0 != ret) {
		return ret;
	}
	return 0;

#undef SGMT_MEMB_CNT
}

/**
 * @brief Read replay body -- pile of encoded segments.
 *
 * @param sgmt_tbl
 * @param buff
 * @param fp
 * @return 0 on success
 */
int read_rep_body(struct tbl *sgmt_tbl, buff_t *buff, FILE *fp)
{
	assert(0 == sgmt_tbl->cnt);
	assert(sgmt_tbl->len > 0);

	int ret;
	sgmt_t *sgmt;

	for (size_t idx = 0;  idx < sgmt_tbl->len;  ++idx) {
		/* allocate segment structure */
		sgmt = (sgmt_t *)malloc(sizeof(*sgmt));
		if (!sgmt) {
			ret = APM_E_NO_MEM;
			goto out;
		}
		sgmt_zero(sgmt);

		ret = read_sgmt_hdr(sgmt, buff, fp);
		if (0 != ret) {
			goto out;
		}

		ret = sgmt_data_prep(&sgmt->ecd_data, sgmt->ecd_size);
		if (0 != ret) {
			goto out;
		}

		ret = read_sgmt_body(sgmt, fp);
		if (0 != ret) {
			goto out;
		}

		ret = tbl_push(sgmt_tbl, sgmt);
		if (0 != ret) {
			goto out;
		}
	}
	return 0;
out:
	free(sgmt);
	return ret;
}
