/**
 *@file init_deinit.h
 *@brief
 *@athor Slavomir Vlcek
 *@copyright GPLv2
 */

#ifndef APM_INIT_DEINIT_H
#define APM_INIT_DEINIT_H

extern int strm_prep(strm_t *strm, const size_t len);
extern void strm_empty(strm_t *strm);
extern size_t strm_len(const struct tbl *sgmt_tbl);

extern void sgmt_free_fn(void *p);
extern int sgmt_data_prep(char **ecd_data, unsigned data_size);
extern void sgmt_zero(sgmt_t *sgmt);

extern int join_scrn_blk_alloc(join_scrn_blk_t **blk);

extern void joiner_free_fn(void *p);
extern int joiner_cmp_fn(void *p1, void *p2);
extern void joiner_zero(joiner_t *joiner);

extern void msgbox_free_fn(void *p);

extern int host_blk_alloc(host_blk_t **blk);

extern int rfnd_alloc(rfnd_t **rfnd);

extern void prsn_free_fn(void *p);
void prsn_zero(prsn_t *p);

extern int body_alloc(body_t **body);

extern void *apm_wc3_init(void);
extern void apm_wc3_deinit(apm_t *apm);

#endif /* APM_INIT_DEINIT_H */
