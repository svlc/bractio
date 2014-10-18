/**
 *@file read.h
 *@brief
 *@athor Slavomir Vlcek
 *@copyright GPLv2
 */

#ifndef BRACT_LOAD_H
#define BRACT_LOAD_H

#include "attr.h"

extern int read_rep_hdr(main_hdr_t *main_hdr, sub_hdr_t *sub_hdr,
			buff_t *buff, FILE *fp);
extern int read_rep_body(struct tbl *sgmt_tbl, buff_t *buff, FILE *fp);

#endif /* BRACT_LOAD_H */


