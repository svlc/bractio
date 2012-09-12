/**
 *@file decode.h
 *@brief
 *@athor Slavomir Vlcek
 *@copyright GPLv2
 */

#ifndef APM_DECODE_H
#define APM_DECODE_H


extern int decode_sgmts(sgmt_tbl_t *tbl, strm_t *strm);

extern void decode_opts_map_creator_str(char *ecd_str_pos, char *dcd_str_pos);


#endif /* APM_DECODE_H */
