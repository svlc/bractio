/**
 *@file decode.c
 *@brief
 *@athor
 *@attention REFACTORIZATION NEEDED
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <zlib.h>

#include "apm.h"


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
        z_strm.avail_in = blk->encod_size;
        /* give over the pointer to encoded buffer */
        z_strm.next_in = blk->encod_strm;


        z_strm.avail_out = blk->decod_size;

        z_strm.next_out = strm->arr + strm->cnt;


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

        strm->cnt += blk->decod_size;
        /* all OK */
        return 0;
}
