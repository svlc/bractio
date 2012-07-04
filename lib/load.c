/**
 *@file load.c
 *@brief
 *@athor Slavomir Vlcek
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


typedef struct aux_t {

        void *const ptr;
        const int size;
} aux_t;


/**
 *@brief Loads sub header (v0) data from file into memory
 */
static int _read_sub_header_v0(sup_t *sup)
{

#define SUB_HEADER_MEMBERS_CNT  6

        /* auxiliary pointers */
        sub_hder_t *sub_hder = &sup->sub_hder;
        char *const buff = sup->core.buff;


        /* read 0x10 bytes */
        VGUARD(0x10 != fread(buff, 1, 0x10, sup->core.fp),
               1, "Subheader could not be read." );


        /* v0 has different size of "patch_ver" */
        uint16_t patch_ver;

        /* this variable will not be saved */
        uint16_t unknown;
        

        /* auxiliary struct array */
        aux_t aux_arr[ SUB_HEADER_MEMBERS_CNT ] = {

                { &unknown,                     sizeof(unknown) },
                { &patch_ver,                   sizeof(patch_ver) },
                { &sub_hder->built_no,          sizeof(sub_hder->built_no) },
                { &sub_hder->player_mode,       sizeof(sub_hder->player_mode) },
                { &sub_hder->rep_len_ms,        sizeof(sub_hder->rep_len_ms) },
                { &sub_hder->hder_CRC32,        sizeof(sub_hder->hder_CRC32) }
        };

        /* go through all subheader members */
        for(int idx = 0, offs = 0;  idx < SUB_HEADER_MEMBERS_CNT;  ++idx) {

                /* copy data from buffer */
                memcpy(aux_arr[idx].ptr, buff + offs, aux_arr[idx].size);

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
static int _read_sub_header_v1(sup_t *sup)
{

        /* auxiliary pointers */
        sub_hder_t *sub_hder = &sup->sub_hder;
        char *const buff = sup->core.buff;


        /* read 0x14 bytes */
        VGUARD(0x14 != fread(buff, 1, 0x14, sup->core.fp),
               1, "Subheader could not be read." );


        /* auxiliary struct array */
        aux_t aux_arr[ SUB_HEADER_MEMBERS_CNT ] = {

                { sub_hder->rls,                4 },
                { &sub_hder->patch_ver,         sizeof(sub_hder->patch_ver) },
                { &sub_hder->built_no,          sizeof(sub_hder->built_no) },
                { &sub_hder->player_mode,       sizeof(sub_hder->player_mode) },
                { &sub_hder->rep_len_ms,        sizeof(sub_hder->rep_len_ms) },
                { &sub_hder->hder_CRC32,        sizeof(sub_hder->hder_CRC32) }
        };


        /* go through all subheader members */
        for(int idx = 0, offs = 0;  idx < SUB_HEADER_MEMBERS_CNT;  ++idx) {

                /* copy data from buffer */
                memcpy(aux_arr[idx].ptr, buff + offs, aux_arr[idx].size);

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
static int _read_main_header(sup_t *sup)
{

#define MAIN_HEADER_MEMBERS_CNT 6


        /* auxiliary pointers */
        main_hder_t *main_hder = &sup->main_hder;
        char *const buff = sup->core.buff;


        /* read 0x30 bytes */
        VGUARD(0x30 != fread(buff, 1, 0x30, sup->core.fp),
               1, "Main header could not be read.");


        /* auxiliary struct array */
        aux_t aux_arr[ MAIN_HEADER_MEMBERS_CNT ] = {

                { main_hder->wc3_str,
                  28 },

                { &main_hder->fir_blk,
                  sizeof(main_hder->fir_blk) },

                { &main_hder->total_file_size,
                  sizeof(main_hder->total_file_size) },

                { &main_hder->hder_ver,
                  sizeof(main_hder->hder_ver) },

                { &main_hder->decod_data_size,
                  sizeof(main_hder->decod_data_size) },

                { &main_hder->encod_blk_cnt,
                  sizeof(main_hder->encod_blk_cnt) }
        };


        /* go through all mainheader members */
        for(int idx = 0, offs = 0;  idx < MAIN_HEADER_MEMBERS_CNT;  ++idx) {

                /* copy data from buffer */
                memcpy(aux_arr[idx].ptr, buff + offs, aux_arr[idx].size);

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
 *@param "sup" Superior data struct that contains main_hder_t, sub_hder_t
 */
static int _read_header(sup_t *sup)
{

        int ret;

        ret = _read_main_header(sup);
        GUARD(0 != ret, ret);


        /* if header is of version "0" */
        if(0x00 == sup->main_hder.hder_ver) {
                ret = _read_sub_header_v0(sup);
                GUARD(0 != ret, ret);

        }
        /* if header is of version "1" */
        else if(0x01 == sup->main_hder.hder_ver) {

                ret =_read_sub_header_v1(sup);
                GUARD(0 != ret, ret);

        } else {
                /* replay is corrupted or something */
                VGUARD(true, 1, "Subheader's version is unvalid.");
        }
        /* all OK */
        return 0;
}

        
/**
 *@brief
 */
static int _load_block_data(core_t *core, blk_t *blk)
{

        /* load encoded chunk from file into block */
        VGUARD(blk->encod_size != fread(blk->encod_strm, 1,
                                        blk->encod_size, core->fp),
               1, "data block could not be read.");

        /* all OK */
        return 0;
}


/**
 *@brief
 */
static int _load_block_header(core_t *core, blk_t *blk)
{

#define BLOCK_MEMBERS_CNT       3


        /* read 0x08 bytes */
        VGUARD(0x08 != fread(core->buff, 1, 0x08, core->fp),
               1, "data block header could not be read.");


        aux_t aux_arr[ BLOCK_MEMBERS_CNT ] = {

                { &blk->encod_size,     sizeof(blk->encod_size) },
                { &blk->decod_size,     sizeof(blk->decod_size) },
                { &blk->unknown,        sizeof(blk->unknown) }
        };


        /* go through all "blk_t" members */
        for(int idx = 0, offs = 0;  idx < BLOCK_MEMBERS_CNT;  ++idx) {

                /* copy data from buffer */
                memcpy(aux_arr[idx].ptr, core->buff + offs, aux_arr[idx].size);

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
static int _load_data_blocks(sup_t *sup)
{

        int ret = _init_block_table(sup);
        GUARD(0 != ret, ret);


        for(unsigned idx = 0;  idx < sup->main_hder.encod_blk_cnt;  ++idx) {

                ret = _load_block_header(&sup->core, &sup->blk_tab.arr[idx]);
                GUARD(0 != ret, ret);

                /* init block based on its header data */
                ret = _init_block(&sup->blk_tab.arr[idx]);
                GUARD(0 != ret, ret);

                /* another block's "encod_strm" was successfully allocated */
                ++sup->blk_tab.cnt;

                ret = _load_block_data(&sup->core, &sup->blk_tab.arr[idx]);
                GUARD(0 != ret, ret);

        }
        /* all OK */
        return 0;
}


/**
 *@brief
 */
static int _load_replay(sup_t *sup)
{

        int ret;

        if(sup->core.req & APM_REQ_HEADER) {

                ret = _read_header(sup);
                GUARD(0 != ret, ret);
        }


        if(sup->core.req & APM_REQ_APM) {

                ret = _load_data_blocks(sup);
                GUARD(0 != ret, ret);

                fclose(sup->core.fp);
                sup->core.fp = NULL;

                /* initialized stream for decoded data */
                _init_stream(sup);

                for(unsigned i = 0;  i < sup->blk_tab.cnt;  ++i) {

                        ret = _decode(&sup->blk_tab.arr[i], &sup->strm);
                        GUARD(0 != ret, ret);
                }
        }
        /* all OK */
        return 0;
}


/**
 *@brief
 */
int apm_wc3_process(sup_t *sup, const request_t req)
{

        int ret;

        ret = _verify_request(req);
        GUARD(0 != ret, ret);

        /* back-up request bitmap */
        sup->core.req = req;

        ret = _load_replay(sup);
        GUARD(0 != ret, ret);


        /* if(sup->core.req & APM_REQ_APM) { */
                
        /*      TODO(); */
        /* } */


        /* all OK */
        return 0;
}
