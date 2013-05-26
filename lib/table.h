/**
 * @file table.h
 * @brief generic array
 * @athor Slavomir Vlcek
 * @copyright GPLv2
 */

#ifndef APM_TABLE_H
#define APM_TABLE_H

typedef void (*free_data_fn_t)(void *p);
typedef int (*cmp_fn_t)(void *p1, void *p2);
typedef void (*print_fn_t) (size_t idx, void *p);

struct tbl {
	void **arr;
	size_t len;
	/* current item count */
	size_t cnt;
	free_data_fn_t free_data_fn;
};

void tbl_sort(struct tbl *t, cmp_fn_t cmp_fn);
int tbl_push(struct tbl *t, void *data);
struct tbl *tbl_alloc(size_t len, free_data_fn_t fn);
void tbl_dealloc(void *p);
void tbl_print(struct tbl *t, print_fn_t print_fn);

#endif /* APM_TABLE_H */
