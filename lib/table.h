/**
 * @file table.h
 * @brief generic array
 * @athor Slavomir Vlcek
 * @copyright GPLv2
 */

#ifndef APM_TABLE_H
#define APM_TABLE_H

typedef void (*tbl_free_data_fn_t)(void *p);
typedef int (*tbl_cmp_fn_t)(void *p1, void *p2);

typedef struct tbl_t {

	void **arr;
	size_t len;
	size_t cnt;

	tbl_free_data_fn_t tbl_free_data_fn;

} tbl_t;


extern void tbl_sort(tbl_t *tbl, tbl_cmp_fn_t fn);
extern int tbl_add_item(tbl_t *tbl, void *data);
extern void tbl_zero(tbl_t *tbl);
extern int tbl_prep(tbl_t *tbl, size_t len, tbl_free_data_fn_t fn);
extern void tbl_empty(tbl_t *tbl);
extern int tbl_alloc(tbl_t **tbl);
extern void tbl_dealloc(tbl_t **tbl);

#endif /* APM_TABLE_H */
