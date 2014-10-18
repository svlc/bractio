/**
 *@file libbract.c
 *@brief Some functions of API.
 *@athor Slavomir Vlcek
 *@copyright GPLv2
 */

#include <stdio.h>		/* fread(3) */
#include <inttypes.h>		/* fixed width integer types */
#include <assert.h>

#include "bract.h"
#include "debug.h"
#include "read.h"
#include "init_deinit.h"
#include "decode.h"
#include "check.h"
#include "str.h"
#include "stream.h"
#include "usage.h"

/**
 * @brief Processes replay file and prepares data that was asked by user.
 *
 * @param p opaque pointer to bract_t
 * @param attr attributes (replay path, settings, ...)
 * @return 0 on success
 */
int bract_wc3_operate(bract_t *bract, bract_wc3_attr_t *attr)
{
	int ret;

	ret = check_task(attr->task);
	if (0 != ret) {
		return BRACT_E_TASK;
	}

	bract->core.task = attr->task;

	/* open passed file ("b" flag is there just for win OS family) */
	bract->core.fp = fopen(attr->path, "rb");
	if (NULL == bract->core.fp) {
		return BRACT_E_FILE_OPENING;
	}

	ret = read_rep_hdr(&bract->main_hdr, &bract->sub_hdr,
			   &bract->core.buff, bract->core.fp);
	if (0 != ret) {
		return ret;
	}

	if (attr->task & BRACT_TASK_ADDTL) {
		body_alloc(&bract->body);
		bract->body->sgmt_tbl = tbl_alloc(bract->main_hdr.ecd_sgmt_cnt,
						sgmt_free_fn);
		if (!bract->body->sgmt_tbl) {
			return -1;
		}

		ret = read_rep_body(bract->body->sgmt_tbl, &bract->core.buff,
				    bract->core.fp);
		if (0 != ret) {
			return ret;
		}

		/* close file */
		fclose(bract->core.fp);
		bract->core.fp = NULL;

		/* prepare stream for decoded data */
		ret = strm_prep(&bract->body->strm,
				strm_len(bract->body->sgmt_tbl));
		if (0 != ret) {
			return ret;
		}

		/* decode all segments using zlib */
		ret = decode_sgmts(bract->body->sgmt_tbl, &bract->body->strm);
		if (0 != ret) {
			return ret;
		}

		/* free segment table resources */
		tbl_dealloc(bract->body->sgmt_tbl);
		bract->body->sgmt_tbl = NULL;

		/* typically there are trailing zeroes at the end of stream,
		 * so set lim right at the end of valid characters */
		bract->body->strm.lim
			= bract->body->strm.arr + bract->main_hdr.dcd_data_size;

		/* allocate rfnd_t structure */
		ret = rfnd_alloc(&bract->rfnd);
		if (0 != ret) {
			return ret;
		}

		ret = process_stream(&bract->body->strm, bract->rfnd,
				     bract->sub_hdr.build);
		if (0 != ret) {
			return ret;
		}

		/* free resources */
		strm_empty(&bract->body->strm);
	}

	if (attr->task & BRACT_TASK_APM) {
		/* compute apm base on player's leave time */
		eval_apm(bract->rfnd->extra.joiner_tbl);
	}
	return 0;
}

/**
 * @brief 
 */
unsigned bract_wc3_getrlsver(bract_t *bract)
{
	assert(bract->core.task & BRACT_TASK_BASIC);

	return bract->sub_hdr.rls;
}

/**
 * @brief 
 */
unsigned bract_wc3_getpatchver(bract_t *bract)
{
	assert(bract->core.task & BRACT_TASK_BASIC);

	return (unsigned)bract->sub_hdr.patch_ver;
}

/**
 * @brief 
 */
unsigned bract_wc3_getbuild(bract_t *bract)
{
	assert(bract->core.task & BRACT_TASK_BASIC);

	return (unsigned)bract->sub_hdr.build;
}

/**
 *@brief
 */
unsigned bract_wc3_getreplen(bract_t *bract)
{
	assert(bract->core.task & BRACT_TASK_BASIC);

	return bract->sub_hdr.rep_len_ms;
}

/**
 * @brief 
 */
char *bract_wc3_getmappath(bract_t *bract)
{
	assert(bract->core.task & BRACT_TASK_ADDTL);

	return bract->rfnd->host_blk->map_path;
}

/**
 * @brief 
 */
unsigned bract_wc3_getmapposcnt(bract_t *bract)
{
	assert(bract->core.task & BRACT_TASK_ADDTL);

	return bract->rfnd->join_scrn_blk->start_spot_cnt;

}

/**
 * @brief 
 */
unsigned bract_wc3_getjoinercnt(bract_t *bract)
{
	assert(bract->core.task & BRACT_TASK_ADDTL);

	return bract->rfnd->extra.joiner_tbl->cnt;
}

/**
 * @brief 
 */
joiner_t *bract_wc3_getjoiner(bract_t *bract, const unsigned no)
{
	return (joiner_t *)bract->rfnd->extra.joiner_tbl->arr[no];
}

/**
 * @brief 
 */
unsigned bract_wc3_getmsgcnt(bract_t *bract)
{
	assert(bract->core.task & BRACT_TASK_ADDTL);

	return bract->rfnd->extra.chat_ls->cnt;
}

/**
 * @brief 
 */
msgbox_t *bract_wc3_getmsg(bract_t *bract, const unsigned no)
{
	assert(bract->core.task & BRACT_TASK_ADDTL);

	return (struct msgbox_t *)ulist_get(bract->rfnd->extra.chat_ls, no);
}
