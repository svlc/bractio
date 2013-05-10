/**
 * @file init_deinit.c
 * @brief
 * @athor Slavomir Vlcek
 * @copyright GPLv2
 */

#include <stdio.h>		/* fread(3) */
#include <stdlib.h>		/* NULL, {m,c}alloc(3), free(3) */
#include <errno.h>		/* errno */
#include <string.h>		/* strerror(3) */
#include <stdbool.h>
#include <assert.h>

#include "rapm.h"
#include "list.h"



/**
 * @brief Inits memory and variables for decoded data stream
 *
 * @todo Consider whether we should compute the total stream length here
 * or use "apm->main_hder.dcd_data_size" instead (but remember
 * that "dcd_data_size" has no zero padding in the last block)
 */
int strm_prep(strm_t *strm, const size_t len)
{
	assert(NULL == strm->arr);
	assert(0 == strm->len);
	assert(0 == strm->cnt);

	/* allocate needed memory */
	strm->arr = (char *)malloc(len);
	if (!strm->arr) {
		return 1;
	}
	strm->len = len;

	/* set position to array start */
	strm->pos = strm->arr;

	/* set pointer to first 'out of range' char */
	strm->lim = strm->arr + len;

	return 0;
}


/**
 * @brief 
 */
static void strm_zero(strm_t *strm)
{
	memset(strm, 0, sizeof(strm_t));
}


/**
 * @brief 
 */
void strm_empty(strm_t *strm)
{
	free(strm->arr);

	strm_zero(strm);
}


/**
 * @brief Compute stream length, including trailing zeroes.
 */
size_t strm_len(const sgmt_tbl_t *tbl)
{
	size_t strm_len = 0;

	/* compute total stream length */
	for (size_t idx = 0;  idx < tbl->cnt;  ++idx) {
		strm_len += (*(sgmt_t *)tbl->arr[idx]).dcd_size;
	}

	return strm_len;
}


/**
 * @brief 
 */
void sgmt_free_fn(void *p)
{
	free(((sgmt_t *)p)->ecd_data);

	free(p);
}


/**
 * @brief Allocate memory chunk for segment.
 */
int sgmt_data_prep(char **ecd_data, unsigned data_size)
{
	assert(NULL == *ecd_data);

	*ecd_data = (char *)malloc(data_size);
	if (!*ecd_data) {
		return 1;
	}
	return 0;
}


/**
 * @brief
 */
void sgmt_zero(sgmt_t *sgmt)
{
	memset(sgmt, 0, sizeof(sgmt_t));
}


/**
 * @brief
 */
int join_scrn_blk_alloc(join_scrn_blk_t **blk)
{
	assert(NULL == *blk);

	*blk = (join_scrn_blk_t *)malloc(sizeof(**blk));
	if (!*blk) {
		return 1;
	}
	tbl_prep(&(*blk)->slot_tbl, 0, free);

	return 0;
}


/**
 * @brief
 */
static void join_scrn_blk_dealloc(join_scrn_blk_t **blk)
{
	assert(*blk);

	tbl_empty(&(*blk)->slot_tbl);

	free(*blk);

	*blk = NULL;
}


/**
 * @brief 
 */
void joiner_free_fn(void *p)
{
	free(((joiner_t *)p)->name);

	free(p);
}


/**
 * @brief 
 */
int joiner_cmp_fn(void *p1, void *p2)
{
	return (int)(((joiner_t *)p1)->id) - (int)(((joiner_t *)p2)->id);
}


/**
 * @brief 
 */
void joiner_zero(joiner_t *joiner)
{
	memset(joiner, 0, sizeof(joiner_t));
}


/**
 * @brief 
 */
void msgbox_free_fn(void *p)
{
	free(((msgbox_t *)p)->msg);

	free(p);
}


/**
 * @brief Allocate "host_blk_t" structure.
 */
int host_blk_alloc(host_blk_t **blk)
{
	assert(NULL == *blk);

	*blk = (host_blk_t *)malloc(sizeof(**blk));
	if (!*blk) {
		return 1;
	}
	memset(*blk, 0, sizeof(host_blk_t));

	/* set indication this person is game host */
	(*blk)->prsn.host = true;

	return 0;
}


/**
 * @brief 
 */
static void host_blk_dealloc(host_blk_t **blk)
{
	assert(*blk);

	free((*blk)->prsn.name);
	free((*blk)->game_name);
	free((*blk)->map_path);
	free((*blk)->game_creator);

	free(*blk);
	*blk = NULL;
}


/**
 * @brief Prepare "extra" structure.
 */
static void extra_prep(extra_t *extra)
{
	tbl_zero(&extra->joiner_tbl);

	extra->chat_ls = NULL;
	extra->action_ls = NULL;
}


/**
 * @brief Allocate rfnd_t structures.
 */
int rfnd_alloc(rfnd_t **rfnd)
{
	assert(NULL == *rfnd);

	*rfnd = (rfnd_t *)malloc(sizeof(**rfnd));
	if (!*rfnd) {
		return 1;
	}
	(*rfnd)->host_blk = NULL;

	host_blk_alloc(&(*rfnd)->host_blk);

	

	(*rfnd)->join_scrn_blk = NULL;
	(*rfnd)->prsn_tbl = NULL;

	extra_prep(&(*rfnd)->extra);

	return 0;
}


/**
 * @brief Function to be passed to tbl_prep().
 */
void prsn_free_fn(void *p)
{
	free(((prsn_t *)p)->name);

	free(p);
}


/**
 * @brief Zero person item.
 */
void prsn_zero(prsn_t *p)
{
	memset(p, 0, sizeof(prsn_t));
}


/**
 * @brief Allocate body structures.
 */
int body_alloc(body_t **body)
{
	assert(NULL == *body);

	*body = (body_t *)malloc(sizeof(**body));
	if (!*body) {
		return 1;
	}	
	/* zero sgmt_tbl_t */
	tbl_zero(&(*body)->sgmt_tbl);

	/* zero strm_t */
	memset(&(*body)->strm, 0, sizeof(strm_t));

	return 0;
}


/**
 * @brief Allocate the most essential structures.
 *
 * @return opaque pointer, NULL on failure
 */
void *apm_wc3_init(void)
{
/* bigger size boosts nothing */
#define BUFF_SIZE	128

	int ret;
	apm_t *apm;

	apm = (apm_t *)malloc(sizeof(*apm));
	if (!apm) {
		return NULL;
	}
	/* allocate "BUFF_SIZE" bytes for buffer */
	ret = buff_prep(&apm->core.buff, BUFF_SIZE);
	if (0 != ret) {
		free(apm);
		return NULL;
	}

	/* init file pointer */
	apm->core.fp = NULL;

	apm->body = NULL;
	apm->rfnd = NULL;

	return (void *)apm;
#undef BUFF_SIZE
}


/**
 * @brief Close the file, free all resources.
 *
 * @note whole "apm_t" structure will be freed
 * so it is meaningless to reset any variables to NULL
 */
void apm_wc3_deinit(apm_t *apm)
{
	/* auxiliary pointers */
	rfnd_t *rfnd = apm->rfnd;
	extra_t *extra = &rfnd->extra;


	if (NULL == apm) {
		return;
	}

	/* if fp is valid (fclose(NULL) is deadly) */
	if (apm->core.fp) {
		fclose(apm->core.fp);
	}

	if (apm->core.buff.arr) {
		buff_empty(&apm->core.buff);
	}

	if (apm->body) {
		tbl_empty(&apm->body->sgmt_tbl);

		if (apm->body->strm.arr) {
			strm_empty(&apm->body->strm);
		}
		free(apm->body);
	}

	if (rfnd) {
		if (rfnd->host_blk) {
			host_blk_dealloc(&rfnd->host_blk);
		}

		if (rfnd->join_scrn_blk) {
			join_scrn_blk_dealloc(&rfnd->join_scrn_blk);
		}

		if (rfnd->prsn_tbl) {
			tbl_dealloc(&rfnd->prsn_tbl);
		}

		tbl_empty(&extra->joiner_tbl);
		if (extra->chat_ls) {
			list_dealloc(&extra->chat_ls);
		}
		if (extra->action_ls) {
			list_dealloc(&extra->action_ls);
		}

		free(rfnd);
	}

	free(apm);
}
