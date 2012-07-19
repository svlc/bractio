/**
 *@file verify.c
 *@brief
 *@athor Slavomir Vlcek
 *@copyright GPLv2
 */

#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "apm.h"
#include "debug.h"

/**
 *@brief 
 */
int _verify_request(const request_t req)
{

        /* if APM_REQ_HEADER bit is not set */
        GUARD(0 == (req & APM_REQ_HEADER), 1);
                
        /* all OK */
        return 0;
}
