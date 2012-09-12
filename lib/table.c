/**
 * @file table.c
 * @brief generic array (of void pointers to save any data type)
 * @athor Slavomir Vlcek
 * @copyright GPLv2
 */

#include <stdlib.h>		/* NULL, {m,c}alloc(3), free(3) */
#include <errno.h>		/* errno */
#include <stdbool.h>
#include <assert.h>
#include <stdio.h>


#include "table.h"

/* int tbl_gap(tbl_t *tbl, etc) */


/**
 * @brief Swap two pointers.
 *
 * @param p1
 * @param p2
 */
void _swap(void **p1, void **p2)
{
	void *aux;

	aux = *p1;
	*p1 = *p2;
	*p2 = aux;
}


/**
 * @brief Sort table items using "selection sort".
 *
 * @param tbl table
 * @param fn pointer to comparison function
 */
void tbl_sort(tbl_t *tbl, tbl_cmp_fn_t fn)
{

	/* outer count */
	size_t o_cnt = tbl->cnt;
	/* inner count */
	size_t i_cnt;

	void **min;

	for (void **o_item = tbl->arr;	o_cnt--;  ++o_item) {

		min = o_item;

		/* notice postfix decr in outer loop condition */
		i_cnt = o_cnt;

		for (void **i_item = o_item + 1;  i_cnt--;  ++i_item) {

			/* if "*i_item" item is lower */
			if (fn(*i_item, *min) < 0) {
				min = i_item;
			}
		}
		/* to not swap two same pointers */
		if (o_item != min) {
			_swap(min, o_item);
		}
	}
}


/**
 * @brief Extend table capacity.
 *
 * @param tbl table
 * @return 0 on success, 1 on memory failure
 */
static int _tbl_expand(tbl_t *tbl)
{
#define TBL_SIZE 10

	void **tmp_arr;
	size_t tmp_len;

	/* if this is initial allocation */
	if (0 == tbl->cnt) {
		tmp_len = TBL_SIZE;
	}
	/* if this is capacity expansion */
	else {
		tmp_len = 2 * tbl->cnt;
	}

	/* try to extend table capacity */
	tmp_arr = (void **)realloc(tbl->arr, tmp_len * sizeof(void *));
	if (NULL == tmp_arr) {
		return 1;
	}

	tbl->arr = tmp_arr;
	tbl->len = tmp_len;
	
	return 0;
#undef TBL_SIZE
}


/**
 * @brief Add exactly one data item to table.
 *
 * @param tbl table
 * @param data data pointer
 * @return 0 on success
 */
int tbl_add_item(tbl_t *tbl, void *data)
{
	int ret;

	if (tbl->cnt >= tbl->len) {
		ret = _tbl_expand(tbl);
		if (0 != ret) {
			return ret;
		}
	}

	/* save data pointer to first unused index */
	tbl->arr[tbl->cnt] = data;
	++tbl->cnt;

	return 0;
}


/**
 * @brief Zero table variables.
 *
 * @param tbl table
 */
void tbl_zero(tbl_t *tbl)
{
	tbl->arr = NULL;
	tbl->len = 0;
	tbl->cnt = 0;
}


/**
 * @brief Prepare table of required length.
 *
 * @param tbl table
 * @param len required table capacity
 * @param fn function to be used for table emptying (@see tbl_empty function)
 * @return 0 on success, 1 on memory failure
 */
int tbl_prep(tbl_t *tbl, size_t len, tbl_free_data_fn_t fn)
{
	tbl_zero(tbl);

	if (len > 0) {
		tbl->arr = (void **)malloc(len * sizeof(void *));
		/* if allocation failed */
		if (NULL == tbl->arr) {
			return 1;
		}
		tbl->len = len;
	}
	/* save pointer to data deallocation */
	tbl->tbl_free_data_fn = fn;

	return 0;
}


/**
 * @brief Free all table resources except table structure.
 * 
 * @param tbl table
 */
void tbl_empty(tbl_t *tbl)
{
	/* while there are still items */
	while (tbl->cnt--) {
		/* free item data */
		tbl->tbl_free_data_fn(tbl->arr[tbl->cnt]);
	}

	free(tbl->arr);
	/* zero all table variables */
	tbl_zero(tbl);
}


/**
 * @brief Allocate tbl_t structure.
 * 
 * @param tbl double pointer to table
 * @return 0 on success, 1 on memory allocation failure
 */
int tbl_alloc(tbl_t **tbl)
{
	assert(NULL == *tbl);

	*tbl = (tbl_t *)malloc(sizeof(tbl_t));
	if (NULL == *tbl) {
		return 1;
	}

	tbl_zero(*tbl);

	return 0;
}


/**
 * @brief Deallocate tbl_t structure.
 *
 * @param tbl double pointer to table
 */
void tbl_dealloc(tbl_t **tbl)
{
	assert(NULL != *tbl);

	tbl_empty(*tbl);
	free(*tbl);
	*tbl = NULL;
}
