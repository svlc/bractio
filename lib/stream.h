/**
 *@file stream.h
 *@brief
 *@athor Slavomir Vlcek
 *@copyright GPLv2
 */

#include "actions.h"

#ifndef APM_STREAM_H
#define APM_STREAM_H

typedef struct blk_nfo_t {

	unsigned id;

	/* can this block be present more times? */
	bool repeat;

	/* must this "id"/these "id"s be always present? */
	bool mandatory;

} blk_nfo_t;


typedef int(*fn_t)(ad_t *ad);


typedef struct fn_box_t {

	fn_t fn;

} fnbox_t;



extern int process_stream(strm_t *strm, rfnd_t *rfnd, const unsigned patch_ver);
extern void eval_apm(struct tbl *joiner_tbl);

#endif /* APM_STREAM_H */
