/**
 *@file rapm.c
 *@brief Some functions of API.
 *@athor Slavomir Vlcek
 *@copyright GPLv2
 */

#include <stdio.h>		/* fread(3) */
#include <inttypes.h>		/* fixed width integer types */
#include <assert.h>

#include "rapm.h"
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
 * @param p opaque pointer to apm_t
 * @param attr attributes (replay path, settings, ...)
 * @return 0 on success
 */
int apm_wc3_operate(apm_t *apm, apm_wc3_attr_t *attr)
{
	int ret;

	ret = check_task(attr->task);
	if (0 != ret) {
		return APM_E_TASK;
	}

	apm->core.task = attr->task;

	/* open passed file ("b" flag is there just for win OS family) */
	apm->core.fp = fopen(attr->path, "rb");
	if (NULL == apm->core.fp) {
		return APM_E_FILE_OPENING;
	}

	ret = read_rep_hdr(&apm->main_hdr, &apm->sub_hdr,
			   &apm->core.buff, apm->core.fp);
	if (0 != ret) {
		return ret;
	}

	if (attr->task & APM_TASK_ADDTL) {
		body_alloc(&apm->body);
		apm->body->sgmt_tbl = tbl_alloc(apm->main_hdr.ecd_sgmt_cnt,
						sgmt_free_fn);
		if (!apm->body->sgmt_tbl) {
			return -1;
		}

		ret = read_rep_body(apm->body->sgmt_tbl, &apm->core.buff,
				    apm->core.fp);
		if (0 != ret) {
			return ret;
		}

		/* close file */
		fclose(apm->core.fp);
		apm->core.fp = NULL;

		/* prepare stream for decoded data */
		ret = strm_prep(&apm->body->strm,
				strm_len(apm->body->sgmt_tbl));
		if (0 != ret) {
			return ret;
		}

		/* decode all segments using zlib */
		ret = decode_sgmts(apm->body->sgmt_tbl, &apm->body->strm);
		if (0 != ret) {
			return ret;
		}

		/* free segment table resources */
		tbl_dealloc(apm->body->sgmt_tbl);
		apm->body->sgmt_tbl = NULL;

		/* typically there are trailing zeroes at the end of stream,
		 * so set lim right at the end of valid characters */
		apm->body->strm.lim
			= apm->body->strm.arr + apm->main_hdr.dcd_data_size;

		/* allocate rfnd_t structure */
		ret = rfnd_alloc(&apm->rfnd);
		if (0 != ret) {
			return ret;
		}

		ret = process_stream(&apm->body->strm, apm->rfnd,
				     apm->sub_hdr.build);
		if (0 != ret) {
			return ret;
		}

		/* free resources */
		strm_empty(&apm->body->strm);
	}

	if (attr->task & APM_TASK_APM) {
		/* compute apm base on player's leave time */
		eval_apm(apm->rfnd->extra.joiner_tbl);
	}
	return 0;
}

/**
 * @brief 
 */
unsigned apm_wc3_getrlsver(apm_t *apm)
{
	assert(apm->core.task & APM_TASK_BASIC);

	return apm->sub_hdr.rls;
}

/**
 * @brief 
 */
unsigned apm_wc3_getpatchver(apm_t *apm)
{
	assert(apm->core.task & APM_TASK_BASIC);

	return (unsigned)apm->sub_hdr.patch_ver;
}

/**
 * @brief 
 */
unsigned apm_wc3_getbuild(apm_t *apm)
{
	assert(apm->core.task & APM_TASK_BASIC);

	return (unsigned)apm->sub_hdr.build;
}

/**
 *@brief
 */
unsigned apm_wc3_getreplen(apm_t *apm)
{
	assert(apm->core.task & APM_TASK_BASIC);

	return apm->sub_hdr.rep_len_ms;
}

/**
 * @brief 
 */
char *apm_wc3_getmappath(apm_t *apm)
{
	assert(apm->core.task & APM_TASK_ADDTL);

	return apm->rfnd->host_blk->map_path;
}

/**
 * @brief 
 */
unsigned apm_wc3_getmapposcnt(apm_t *apm)
{
	assert(apm->core.task & APM_TASK_ADDTL);

	return apm->rfnd->join_scrn_blk->start_spot_cnt;

}

/**
 * @brief 
 */
unsigned apm_wc3_getjoinercnt(apm_t *apm)
{
	assert(apm->core.task & APM_TASK_ADDTL);

	return apm->rfnd->extra.joiner_tbl->cnt;
}

/**
 * @brief 
 */
joiner_t *apm_wc3_getjoiner(apm_t *apm, const unsigned no)
{
	return (joiner_t *)apm->rfnd->extra.joiner_tbl->arr[no];
}

/**
 * @brief 
 */
unsigned apm_wc3_getmsgcnt(apm_t *apm)
{
	assert(apm->core.task & APM_TASK_ADDTL);

	return apm->rfnd->extra.chat_ls->cnt;
}

/**
 * @brief 
 */
msgbox_t *apm_wc3_getmsg(apm_t *apm, const unsigned no)
{
	assert(apm->core.task & APM_TASK_ADDTL);

	return (struct msgbox_t *)ulist_get(apm->rfnd->extra.chat_ls, no);
}
