/**
 *@file decode.c
 *@brief
 *@athor
 *@copyright
 *@attention REFACTORIZATION NEEDED
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <zlib.h>

#include "apm.h"
#include "debug.h"


int _decode(blk_t *blk, strm_t *strm)
{

        int ret;
        z_stream z_strm;

        z_strm.zalloc = Z_NULL;
        z_strm.zfree = Z_NULL;
        z_strm.opaque = Z_NULL;


        z_strm.avail_in = 0;
        z_strm.next_in = Z_NULL;

        /* init z_strm */
        ret = inflateInit(&z_strm);
        if(Z_OK != ret) {
                return ret;
        }

        /* inform about size of chunk we want decode */
        z_strm.avail_in = blk->ecd_size;
        /* give over the pointer to encoded buffer */
        z_strm.next_in = (unsigned char *)blk->ecd_strm;


        z_strm.avail_out = blk->dcd_size;

        z_strm.next_out = (unsigned char *)strm->arr + strm->cnt;


        /* from "next_in" to "next_out" */
        ret = inflate(&z_strm, Z_NO_FLUSH);

        /* should never happen */
        assert(ret != Z_STREAM_ERROR);

        /* we always know the decoded stream size,
         * so this should always be "0" or decoded
         * stream size info from replay block header was wrong */
        assert(0 == z_strm.avail_out);
        assert(Z_STREAM_END != ret);


        switch(ret) {

        case Z_NEED_DICT:
                ret = Z_DATA_ERROR;

        case Z_DATA_ERROR:      /* if stream is corrupt/unrecognized by zlib */

        case Z_MEM_ERROR:       /* if not enough memory */
                inflateEnd(&z_strm);
                return 1;
        }

        inflateEnd(&z_strm);

        strm->cnt += blk->dcd_size;
        /* all OK */
        return 0;
}

/**
 *@brief
 */
void _decode_opts_map_creator_str(char *ecd_str_pos, char *dcd_str_pos)
{

	assert(NULL != ecd_str_pos);
	assert(NULL != dcd_str_pos);

	unsigned char key;
	unsigned char bit = 0;

	/* iterate until '\0' is found */
	while(*ecd_str_pos) {

		/* if this is key-bit */
		if(0 == bit) {
			/* save it */
			key = *ecd_str_pos;

			/* 0000 0010 */
			bit = (1 << 1);
		} else {
			*dcd_str_pos = *ecd_str_pos;
			if(0 == (key & bit)) {
				--*dcd_str_pos;
			}

			++dcd_str_pos;
			/* left shift until bit value overwflows to 0 */
			bit <<= 1;
		}
		++ecd_str_pos;
	}
	/* make it string */
	*dcd_str_pos = '\0';
}

