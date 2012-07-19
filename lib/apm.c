/**
 *@file apm.c
 *@brief
 *@athor Slavomir Vlcek
 *@copyright GPLv2
 */

#include <stdio.h>              /* fread(3) */
#include <inttypes.h>           /* fixed width integer types */

#include "apm.h"


/**
 *@brief 
 */
char *apm_wc3_get_release(apm_t *apm)
{
        /* check whether "APM_REQ_HEADER"
         * was requested */


        return apm->sub_hder.rls;
}


unsigned apm_wc3_get_patch_version(apm_t *apm)
{
        /* check whether "APM_REQ_HEADER"
         * was requested */

        return (unsigned)apm->sub_hder.patch_ver;
}


/**
 *@brief
 */
unsigned apm_wc3_get_length(apm_t *apm)
{
        /* check whether "APM_REQ_HEADER"
         * was requested */


        return (unsigned)apm->sub_hder.rep_len_ms;
}


char *apm_wc3_get_map_path(apm_t *apm)
{
        if(apm->core.req & APM_REQ_INFO) {
	        return apm->info->map_path;
	}

        return NULL;
}


unsigned apm_wc3_get_map_position_cnt(apm_t *apm)
{

        if(apm->core.req & APM_REQ_INFO) {
	        return (unsigned)apm->info->game_start_rec->start_spot_cnt;
	}

        return 0;
}

char *apm_wc3_get_hostname(apm_t *apm)
{
        if(apm->core.req & APM_REQ_INFO) {
	        return apm->info->game_creator;
	}

        return NULL;
}

unsigned apm_wc3_get_game_speed(apm_t *apm)
{
        if(apm->core.req & APM_REQ_INFO) {
	        return (unsigned)apm->info->game_opts.speed;
	}

        return 0;
}

