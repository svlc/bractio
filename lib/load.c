/**
 *@file load.c
 *@brief
 *@athor Slavomir Vlcek
 *@copyright GPLv2
 */

#include <stdio.h>              /* fread(3) */
#include <stdlib.h>             /* NULL */
#include <string.h>             /* memcpy(3), strerror(3) */
#include <inttypes.h>           /* fixed width integer types */
#include <errno.h>
#include <stdbool.h>


#include "load.h"
#include "debug.h"
#include "apm.h"
#include "init_deinit.h"
#include "decode.h"
#include "verify.h"
#include "str.h"
#include "stream.h"


/**
 *@brief Loads sub header (v0) data from file into memory
 */
static int _read_sub_header_v0(apm_t *apm)
{

#define SUB_HEADER_MEMBERS_CNT  6

        /* auxiliary pointers */
        sub_hder_t *sub_hder = &apm->sub_hder;
        char *const buff = apm->core.buff;


        /* read 0x10 bytes */
        GUARD(0x10 != fread(buff, 1, 0x10, apm->core.fp), 1);


        /* v0 has different size of "patch_ver" */
        uint16_t patch_ver;

        /* this variable will not be saved */
        uint16_t unknown;
        

        /* auxiliary struct array */
        aux_t aux_arr[ SUB_HEADER_MEMBERS_CNT ] = {

                { &unknown,                     sizeof(unknown) },
                { &patch_ver,                   sizeof(patch_ver) },
                { &sub_hder->build_no,          sizeof(sub_hder->build_no) },
                { &sub_hder->player_mode,       sizeof(sub_hder->player_mode) },
                { &sub_hder->rep_len_ms,        sizeof(sub_hder->rep_len_ms) },
                { &sub_hder->hder_CRC32,        sizeof(sub_hder->hder_CRC32) }
        };

        /* go through all subheader members */
        for(int idx = 0, offs = 0;  idx < SUB_HEADER_MEMBERS_CNT;  ++idx) {

                /* copy data from buffer */
                memcpy(aux_arr[idx].dest, buff + offs, aux_arr[idx].size);

                /* shift buffer offset */
                offs += aux_arr[idx].size;
        }


        sub_hder->patch_ver = (uint16_t)patch_ver;

        /* all OK */
        return 0;
}


/**
 *@brief
 */
static int _read_sub_header_v1(apm_t *apm)
{

        /* auxiliary pointers */
        sub_hder_t *sub_hder = &apm->sub_hder;
        char *const buff = apm->core.buff;


        /* read 0x14 bytes */
        GUARD(0x14 != fread(buff, 1, 0x14, apm->core.fp), 1);


        /* auxiliary struct array */
        aux_t aux_arr[ SUB_HEADER_MEMBERS_CNT ] = {

                { sub_hder->rls,                4 },
                { &sub_hder->patch_ver,         sizeof(sub_hder->patch_ver) },
                { &sub_hder->build_no,          sizeof(sub_hder->build_no) },
                { &sub_hder->player_mode,       sizeof(sub_hder->player_mode) },
                { &sub_hder->rep_len_ms,        sizeof(sub_hder->rep_len_ms) },
                { &sub_hder->hder_CRC32,        sizeof(sub_hder->hder_CRC32) }
        };


        /* go through all subheader members */
        for(int idx = 0, offs = 0;  idx < SUB_HEADER_MEMBERS_CNT;  ++idx) {

                /* copy data from buffer */
                memcpy(aux_arr[idx].dest, buff + offs, aux_arr[idx].size);

                /* shift buffer offset */
                offs += aux_arr[idx].size;
        }


        /* to make "rls" string */
        sub_hder->rls[4] = '\0';

        /* string is saved reversed in replay file */
        _rev_char_seq(sub_hder->rls, 4);

        /* all OK */
        return 0;
#undef SUB_HEADER_MEMBERS_CNT
}


/**
 *@brief Loads main header data from file into memory
 */
static int _read_main_header(apm_t *apm)
{

#define MAIN_HEADER_MEMBERS_CNT 6


        /* auxiliary pointers */
        main_hder_t *main_hder = &apm->main_hder;
        char *const buff = apm->core.buff;


        /* read 0x30 bytes */
        GUARD(0x30 != fread(buff, 1, 0x30, apm->core.fp), 1);


        /* auxiliary struct array */
        aux_t aux_arr[ MAIN_HEADER_MEMBERS_CNT ] = {

                { main_hder->wc3_str, 28 },

                { &main_hder->fir_blk, sizeof(main_hder->fir_blk) },

                { &main_hder->total_file_size,
                  sizeof(main_hder->total_file_size) },

                { &main_hder->hder_ver, sizeof(main_hder->hder_ver) },

                { &main_hder->dcd_data_size, sizeof(main_hder->dcd_data_size) },

                { &main_hder->ecd_blk_cnt, sizeof(main_hder->ecd_blk_cnt) }
        };


        /* go through all mainheader members */
        for(int idx = 0, offs = 0;  idx < MAIN_HEADER_MEMBERS_CNT;  ++idx) {

                /* copy data from buffer */
                memcpy(aux_arr[idx].dest, buff + offs, aux_arr[idx].size);

                /* shift buffer offset */
                offs += aux_arr[idx].size;
        }
        /* all OK */
        return 0;

#undef MAIN_HEADER_MEMBERS_CNT
}


/**
 *@brief Reads main header and subheader
 *
 *@param "apm" data struct that contains main_hder_t, sub_hder_t
 */
static int _read_header(apm_t *apm)
{

        int ret;

        ret = _read_main_header(apm);
        GUARD(0 != ret, ret);


        /* if header is of version "0" */
        if(0x00 == apm->main_hder.hder_ver) {
                ret = _read_sub_header_v0(apm);
                GUARD(0 != ret, ret);

        }
        /* if header is of version "1" */
        else if(0x01 == apm->main_hder.hder_ver) {

                ret =_read_sub_header_v1(apm);
                GUARD(0 != ret, ret);

        } else {
                /* replay is corrupted or something */
		return 1;
        }
        /* all OK */
        return 0;
}

        
/**
 *@brief
 */
static int _read_block_data(core_t *core, blk_t *blk)
{

        /* load encoded chunk from file into block */
        GUARD(blk->ecd_size != fread(blk->ecd_strm, 1,
				     blk->ecd_size, core->fp), 1);

        /* all OK */
        return 0;
}


/**
 *@brief
 */
static int _read_block_header(core_t *core, blk_t *blk)
{

#define BLOCK_MEMBERS_CNT       3


        /* read 0x08 bytes */
        GUARD(0x08 != fread(core->buff, 1, 0x08, core->fp), 1);


        aux_t aux_arr[ BLOCK_MEMBERS_CNT ] = {

                { &blk->ecd_size,     sizeof(blk->ecd_size) },
                { &blk->dcd_size,     sizeof(blk->dcd_size) },
                { &blk->unknown,        sizeof(blk->unknown) }
        };


        /* go through all "blk_t" members */
        for(int idx = 0, offs = 0;  idx < BLOCK_MEMBERS_CNT;  ++idx) {

                /* copy data from buffer */
                memcpy(aux_arr[idx].dest, core->buff + offs, aux_arr[idx].size);

                /* shift buffer offset */
                offs += aux_arr[idx].size;
        }
        /* all OK */
        return 0;

#undef BLOCK_MEMBERS_CNT
}


/**
 *@brief
 */
static int _read_data_blocks(apm_t *apm)
{

        int ret = _init_block_table(apm);
        GUARD(0 != ret, ret);


        for(unsigned idx = 0;  idx < apm->main_hder.ecd_blk_cnt;  ++idx) {

                ret = _read_block_header(&apm->core, &apm->blk_tab.arr[idx]);
                GUARD(0 != ret, ret);

                /* init block based on its header data */
                ret = _init_block(&apm->blk_tab.arr[idx]);
                GUARD(0 != ret, ret);

                /* another block's "ecd_strm" was successfully allocated */
                ++apm->blk_tab.cnt;

                ret = _read_block_data(&apm->core, &apm->blk_tab.arr[idx]);
                GUARD(0 != ret, ret);

        }
        /* all OK */
        return 0;
}


/**
 *@brief
 */
static int _read_replay(apm_t *apm)
{

        int ret;

        if(apm->core.req & APM_REQ_HEADER) {

                ret = _read_header(apm);
                GUARD(0 != ret, ret);
        }


        if(apm->core.req & APM_REQ_APM) {

                ret = _read_data_blocks(apm);
                GUARD(0 != ret, ret);

                fclose(apm->core.fp);
                apm->core.fp = NULL;

                /* initialized stream for decoded data */
                _init_stream(apm);

                for(unsigned i = 0;  i < apm->blk_tab.cnt;  ++i) {

                        ret = _decode(&apm->blk_tab.arr[i], &apm->strm);
                        GUARD(0 != ret, ret);
                }
        }
        /* all OK */
        return 0;
}


/**
 *@brief
 */
int apm_wc3_process(apm_t *apm, const request_t req)
{

        int ret;

        ret = _verify_request(req);
        GUARD(0 != ret, ret);

        /* back-up request bitmap */
        apm->core.req = req;

        ret = _read_replay(apm);
        GUARD(0 != ret, ret);


        if(apm->core.req & APM_REQ_INFO) {

		ret = _init_info(apm);
	        GUARD(0 != ret, ret);

		ret = _process_info(apm);
	        GUARD(0 != ret, ret);

        }
        /* all OK */
        return 0;
}
