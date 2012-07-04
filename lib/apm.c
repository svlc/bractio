/**
 *@file apm.c
 *@brief
 *@athor Slavomir Vlcek
 */

#include <stdio.h>              /* fread(3) */
#include <inttypes.h>           /* fixed width integer types */

#include "apm.h"


/**
 *@brief 
 */
char *apm_wc3_get_release(sup_t *sup)
{
        /* check whether "APM_REQ_HEADER"
         * was requested */


        return sup->sub_hder.rls;
}


/**
 *@brief
 */
unsigned apm_wc3_get_length(sup_t *sup)
{
        /* check whether "APM_REQ_HEADER"
         * was requested */


        return (unsigned)sup->sub_hder.rep_len_ms;
}
