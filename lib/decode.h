/**
 *@file decode.h
 *@brief
 *@athor Slavomir Vlcek
 *@copyright GPLv2
 */

#ifndef BRACT_DECODE_H
#define BRACT_DECODE_H

extern int decode_sgmts(struct tbl *sgmt_tbl, strm_t *strm);
extern void decode_opts_map_creator_str(char *ecd_str_pos, char *dcd_str_pos);

#endif /* BRACT_DECODE_H */
