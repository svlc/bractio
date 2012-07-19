/**
 *@file init_deinit.c
 *@brief
 *@athor Slavomir Vlcek
 *@copyright GPLv2
 */

#include <stdio.h>              /* fread(3) */
#include <stdlib.h>             /* NULL, {m,c}alloc(3), free(3) */
#include <errno.h>              /* errno */
#include <string.h>             /* strerror(3) */
#include <stdbool.h>
#include <assert.h>

#include "apm.h"
#include "debug.h"              /* GUARD(), ... */



/**
 *@brief Inits memory and variables for decoded data stream
 *
 *@todo Consider whether we should compute the total stream length here
 *or use "apm->main_hder.dcd_data_size" instead (but remember
 *that "dcd_data_size" has no zero padding in the last block)
 */
int _init_stream(apm_t *apm)
{
        
        assert(NULL == apm->strm.arr);
        assert(0 == apm->strm.len);
        assert(0 == apm->strm.cnt);

        int strm_len = 0;

        /* compute total stream length */
        for(unsigned i = 0;  i < apm->blk_tab.cnt;  ++i) {

                strm_len += apm->blk_tab.arr[i].dcd_size;
        }

        /* allocate needed memory */
        MALLOC(apm->strm.arr, --, strm_len, return 1);

        apm->strm.len = strm_len;

	/* set position to array start */
	apm->strm.pos = apm->strm.arr;

	/* set pointer to first 'out of range' char */
	apm->strm.limit = apm->strm.arr + strm_len;

        /* all OK */
        return 0;
}

/**
 *@brief Allocates memory for blk_tab_t array, sets variables
 *
 *@param apm
 *
 *@attention Block count gets inited to "0" in apm_wc3_init()
 *@attention It is essential (see apm_wc3_deinit()) to set
 *all "enc_strm" pointers to NULL (by calloc())
 */
int _init_block_table(apm_t *apm)
{

        /* allocate zeroed memory */
	CALLOC(apm->blk_tab.arr, --,
	       apm->main_hder.ecd_blk_cnt * sizeof(blk_t), return 1);

        /* set the right array length */
        apm->blk_tab.len = apm->main_hder.ecd_blk_cnt;

        /* all OK */
        return 0;
}

/**
 *@brief
 */
int _init_block(blk_t *blk)
{
	MALLOC(blk->ecd_strm, --, blk->ecd_size, return 1);

        /* all OK */
        return 0;
}


/**
 *@brief
 */
int _prolong_player_table(player_tab_t *player_tab)
{

	/* temporary length */
	int tmp_len = 2 * player_tab->len;
	player_rec_t **tmp_arr = NULL;

	MALLOC(tmp_arr, --, tmp_len * sizeof(player_rec_t), return 1);

	/* copy valid pointers */
	memcpy(tmp_arr, player_tab->arr, player_tab->len);

	/* free old pointer array */
	free(player_tab->arr);

	/* assign new array pointer */
	player_tab->arr = tmp_arr;

	/* update length */
	player_tab->len = tmp_len;

	/* all OK */
	return 0;
}

/**
 *@brief
 */
int _init_player_table(apm_t *apm)
{
#define MAX_PLAYER_SLOT_CNT 12

	assert(NULL == apm->info->player_tab.arr);
	assert(0 == apm->info->player_tab.len);
	assert(0 == apm->info->player_tab.cnt);


	MALLOC(apm->info->player_tab.arr, --,
	       MAX_PLAYER_SLOT_CNT * sizeof(int **), return 1);

	/* note down the array length */
	apm->info->player_tab.len = MAX_PLAYER_SLOT_CNT;

	return 0;


}


/**
 *@brief
 */
int _init_slot_table(apm_t *apm, const int len)
{

	assert(NULL == apm->info->game_start_rec->slot_tab.arr);
	assert(0 == apm->info->game_start_rec->slot_tab.cnt);
	assert(0 == apm->info->game_start_rec->slot_tab.len);

	/* if "cnt" is unreasonable */
	if(len < 1 || len > MAX_PLAYER_SLOT_CNT) {
		return 1;
	}


	MALLOC(apm->info->game_start_rec->slot_tab.arr, --,
	       len * sizeof(slot_rec_t), return 1);

	apm->info->game_start_rec->slot_tab.len = len;
	apm->info->game_start_rec->slot_tab.cnt = 0;

	
	return 0;
#undef MAX_PLAYER_SLOT_CNT
}


/**
 *@brief
 */
int _init_game_start_rec(apm_t *apm)
{

	assert(NULL == apm->info->game_start_rec);

	MALLOC(apm->info->game_start_rec, --,
	       sizeof(game_start_rec_t), return 1);

	apm->info->game_start_rec->slot_tab.arr = NULL;
	apm->info->game_start_rec->slot_tab.len = 0;
	apm->info->game_start_rec->slot_tab.cnt = 0;

	return 0;
}


/**
 *@brief Allocates array of pointers
 */
int _init_info(apm_t *apm)
{

	MALLOC(apm->info, --, sizeof(info_t), return 1);

	apm->info->player_tab.arr = NULL;
	apm->info->player_tab.len = 0;
	apm->info->player_tab.cnt = 0;

	apm->info->game_name = NULL;
	apm->info->map_path = NULL;
	apm->info->game_creator = NULL;

	apm->info->game_start_rec = NULL;

        /* all OK */
        return 0;
}


/**
 *@brief Allocates memory and tries to open given file
 */
apm_t *apm_wc3_init(const char *path)
{
/* bigger size boosts nothing */
#define BUFF_SIZE       128

        apm_t *apm;
	MALLOC(apm, --, sizeof(apm_t), return NULL);

        /* allocate "BUFF_SIZE" bytes for buffer */
	MALLOC(apm->core.buff, --, BUFF_SIZE, free(apm); return NULL);

        apm->core.buff_len = BUFF_SIZE;

        /* init blk_tab_t */
        apm->blk_tab.arr = NULL;
        apm->blk_tab.len = 0;
        apm->blk_tab.cnt = 0;

        /* init strm_t */
        apm->strm.arr = NULL;
        apm->strm.len = 0;
        apm->strm.cnt = 0;

	/* will be allocated later */
	apm->info = NULL;

        /* only windows OS requires "b" flag for binary files */
        apm->core.fp = fopen(path, "rb");
        /* check if file opening was successful */
        if(NULL == apm->core.fp) {

                free(apm->core.buff);
                free(apm);
		return NULL;
        }

        return apm;
#undef BUFF_SIZE
}


/**
 *@brief 
 */
static void _deinit_info(info_t *info)
{

	if(NULL == info) {
		return;
	}

	while(info->player_tab.cnt) {

		--info->player_tab.cnt;

		free(info->player_tab.arr[info->player_tab.cnt]->player_name);

		free(info->player_tab.arr[info->player_tab.cnt]);
	}

	free(info->player_tab.arr);

	free(info->game_name);

	free(info->map_path);

	free(info->game_creator);

	if(info->game_start_rec) {

		free(info->game_start_rec->slot_tab.arr);

		free(info->game_start_rec);
	}

	free(info);

}


/**
 *@brief Closes the file, frees all resources
 *
 *@attention whole "apm_t" structure will be freed
 *so it is meaningless to reset all variables but "apm" to "0"
 */
void apm_wc3_deinit(apm_t *apm)
{

        /* if fp is valid (fclose(NULL) is deadly) */
        if(apm->core.fp) {
                fclose(apm->core.fp);
        }

        /* auxiliary pointer */
        blk_tab_t *blk_tab = &apm->blk_tab;

        /* go through all initialized blk_tab records */
        while(blk_tab->cnt) {

                --blk_tab->cnt;

                /* some block streams may be NULL */
                free(blk_tab->arr[ blk_tab->cnt ].ecd_strm);
        }

        /* possibly free block table array */
        free(blk_tab->arr);

        /* possibly free decoded stream array */
        free(apm->strm.arr);


        /* free buffer */
        free(apm->core.buff);

	if(apm->info) {
		_deinit_info(apm->info);
	}

        free(apm);
        apm = NULL;

}
