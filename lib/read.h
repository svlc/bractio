/**
 *@file read.h
 *@brief
 *@athor Slavomir Vlcek
 *@copyright GPLv2
 */

#ifndef APM_LOAD_H
#define APM_LOAD_H

#include "attr.h"

extern int read_rep_hdr(main_hdr_t *main_hdr, sub_hdr_t *sub_hdr,
			buff_t *buff, FILE *fp);
extern int read_rep_body(sgmt_tbl_t *sgmt_tbl, buff_t *buff, FILE *fp);

#endif /* APM_LOAD_H */


