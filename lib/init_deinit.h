/**
 *@file init_deinit.h
 *@brief
 *@athor Slavomir Vlcek
 */

#ifndef APM_INIT_DEINIT_H
#define APM_INIT_DEINIT_H


extern int _init_stream(sup_t *sup);

extern int _init_block_table(sup_t *sup);

extern int _init_block(blk_t *blk);

extern sup_t *apm_wc3_init(const char *path);

extern void apm_wc3_deinit(sup_t *sup);


#endif /* APM_INIT_DEINIT_H */
