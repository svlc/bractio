/**
 *@file init_deinit.c
 *@brief
 *@athor Slavomir Vlcek
 */

#include <stdio.h>              /* fread(3) */
#include <stdlib.h>             /* NULL, {m,c}alloc(3), free(3) */
#include <errno.h>              /* errno */
#include <string.h>             /* strerror(3) */
#include <stdbool.h>
#include <assert.h>

#include "apm.h"
#include "debug.h"              /* VGUARD(), GUARD() */

/* bigger size boosts nothing */
#define BUFF_SIZE       128



/**
 *@brief Inits memory and variables for decoded data stream
 *
 *@todo Consider whether we should compute the total stream length here
 *or use "sup->main_hder.decod_data_size" instead (but remember
 *that "decod_data_size" has no zero padding in the last block)
 */
int _init_stream(sup_t *sup)
{
        
        assert(NULL == sup->strm.arr);
        assert(0 == sup->strm.len);
        assert(0 == sup->strm.cnt);

        int strm_len = 0;

        /* compute total stream length */
        for(unsigned i = 0;  i < sup->blk_tab.cnt;  ++i) {

                strm_len += sup->blk_tab.arr[i].decod_size;
        }

        /* allocate needed memory */
        sup->strm.arr = malloc(strm_len);
        VGUARD(NULL == sup->strm.arr, 1, "Not enough memory.\n");

        sup->strm.len = strm_len;

        /* all OK */
        return 0;
}


/**
 *@brief Allocates memory for blk_tab_t array, sets variables
 *
 *@param sup
 *
 *@attention Block count gets inited to "0" in apm_wc3_init()
 *@attention It is essential (see apm_wc3_deinit()) to set
 *all "encod_strm" pointers to NULL (by calloc())
 */
int _init_block_table(sup_t *sup)
{

        /* allocate zeroed memory */
        sup->blk_tab.arr = calloc(sup->main_hder.encod_blk_cnt, sizeof(blk_t));
        VGUARD(NULL == sup->blk_tab.arr,
               1, "Not enough memory.\n");

        /* set the right array length */
        sup->blk_tab.len = sup->main_hder.encod_blk_cnt;

        /* all OK */
        return 0;
}


/**
 *@brief
 */
int _init_block(blk_t *blk)
{

        blk->encod_strm = malloc(blk->encod_size);
        VGUARD(NULL == blk->encod_strm,
               1, "Not enough memory.\n");

        /* all OK */
        return 0;
}


/**
 *@brief Allocates memory and tries to open given file
 */
sup_t *apm_wc3_init(const char *path)
{

        sup_t *sup = malloc(sizeof(sup_t));
        VGUARD(NULL == sup, NULL, "Not enough memory.\n");

        /* allocate "BUFF_SIZE" bytes for buffer */
        sup->core.buff = malloc(BUFF_SIZE);
        if(NULL == sup->core.buff) {

                free(sup);
                VGUARD(true, NULL, "Not enough memory.\n");
        }


        sup->core.buff_len = BUFF_SIZE;

        /* init blk_tab_t */
        sup->blk_tab.arr = NULL;
        sup->blk_tab.len = 0;
        sup->blk_tab.cnt = 0;

        /* init strm_t */
        sup->strm.arr = NULL;
        sup->strm.len = 0;
        sup->strm.cnt = 0;

        /* only windows OS requires "b" flag for binary files */
        sup->core.fp = fopen(path, "rb");
        /* check if file opening was successful */
        if(NULL == sup->core.fp) {

                free(sup->core.buff);
                free(sup);
                VGUARD(true, NULL, "File opening failed.\n");
        }

        return sup;
}


/**
 *@brief Closes the file, frees all resources
 *
 *@attention whole "sup_t" structure will be freed
 *so it is meaningless to reset all variables but "sup" to "0"
 */
void apm_wc3_deinit(sup_t *sup)
{

        /* if fp is valid (fclose(NULL) is deadly) */
        if(sup->core.fp) {
                fclose(sup->core.fp);
        }

        /* auxiliary pointer */
        blk_tab_t *blk_tab = &sup->blk_tab;

        /* go through all initialized blk_tab records */
        while(blk_tab->cnt) {

                --blk_tab->cnt;

                /* some block streams may be NULL */
                free(blk_tab->arr[ blk_tab->cnt ].encod_strm);
        }

        /* possibly free block table array */
        free(blk_tab->arr);

        /* possibly free decoded stream array */
        free(sup->strm.arr);


        /* free buffer */
        free(sup->core.buff);

        free(sup);
        sup = NULL;

}
