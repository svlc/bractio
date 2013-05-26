/**
 * @file table.c
 * @brief generic array (of void pointers to save any data type)
 * @athor Slavomir Vlcek
 * @copyright GPLv2
 */

#include <stdlib.h>		/* NULL, {m,c}alloc(3), free(3) */
#include <stdbool.h>
#include <assert.h>

#include "table.h"

#define DEFAULT_TBL_SIZE 10

/**
 * @brief Sort table using "insertion sort".
 */
void tbl_sort(struct tbl *t, cmp_fn_t cmp_fn)
{
	void **first = t->arr;
	void **brk = t->arr + t->cnt;
	/* pointer for outer loop */
	void **o;

	for (o = first + 1;  o < brk;  ++o) {
		/* pointer for inner loop */
		void **i;
		void *tmp = *o;

		for (i = o - 1;  cmp_fn(*i, tmp) > 0 && i >= first; --i) {
			*(i + 1) = *i;
		}
		*(i + 1) = tmp;
	}
}

/**
 * @brief Extend table capacity.
 *
 * @param tbl table
 * @return 0 on success, 1 on memory failure
 */
static int tbl_expand(struct tbl *t)
{
	void **tmp_arr;
	size_t tmp_len;
	int ret;

	/* if this is initial allocation */
	if (0 == t->cnt)
		tmp_len = DEFAULT_TBL_SIZE;
	else
		tmp_len = 2 * t->cnt;

	tmp_arr = (void **)realloc(t->arr, tmp_len * sizeof(void *));
	if (!tmp_arr) {
		ret = -1;
		goto out;
	}
	t->arr = tmp_arr;
	t->len = tmp_len;
	ret = 0;
out:
	return ret;
}

/**
 * @brief Push data at first empty slot.
 *
 * @param t table
 * @param data
 * @return 0 on success
 */
int tbl_push(struct tbl *t, void *data)
{
	int ret;

	if (t->cnt >= t->len) {
		ret = tbl_expand(t);
		if (0 != ret) {
			ret = -1;
			goto out;
		}
	}
	/* save data pointer at first unused slot */
	t->arr[t->cnt++] = data;
	ret = 0;
out:
	return ret;
}

/**
 * @brief Allocate "tbl" structure.
 * @param len length of array, 0 is accepted
 * @param fn
 */
struct tbl *tbl_alloc(size_t len, free_data_fn_t fn)
{
	struct tbl *t;

	t = (struct tbl *)malloc(sizeof(*t));
	if (!t)
		goto err;
	if (len > 0) {
		t->arr = (void **)malloc(len * sizeof(void *));
		if (!t->arr) {
			goto err;
		}
	} else {
		t->arr = NULL;
	}
	t->len = len;
	t->cnt = 0;
	t->free_data_fn = fn;
	return t;
err:
	tbl_dealloc(t);
	return t;
}

/**
 * @brief Deallocate "tbl" structure.
 */
void tbl_dealloc(void *p)
{
	struct tbl *t = (struct tbl *)p;

	if (!t)
		return;

	/* if we have valid function pointer */
	if (t->free_data_fn) {
		while (t->cnt)
			t->free_data_fn(t->arr[--t->cnt]);
	}
	free(t->arr);
	free(t);
}

/**
 * @brief Print all items.
 * @note Not very efficient, meant just for debugging.
 */
void tbl_print(struct tbl *t, print_fn_t print_fn)
{
	void **cur = t->arr;
	void **brk = t->arr + t->cnt;
	size_t idx = 0;

	while (cur < brk)
		print_fn(idx++, cur++);
}
