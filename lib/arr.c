/**
 * @file arr.c
 * @brief Generic array.
 * @details Suitable if the length of array is known in advance
 * and is not to be ever changed (extendibility would be very expensive).
 * @athor Slavomir Vlcek
 * @copyright GPLv2
 */

#include <stdlib.h>		/* NULL, {m,c,re}alloc(3), free(3) */
#include <errno.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>

#include "arr.h"

/**
 * @brief Add some data to some index.
 * 
 * @param arr array
 * @param idx index
 * @param data
 */
void arr_add_item(struct arr *arr, size_t idx, void *data)
{
	assert(idx < arr->cnt);

	memcpy((char *)arr->ptr + (idx * arr->item_size), data, arr->item_size);
}

/**
 * @brief Get the pointer to item.
 *
 * @param arr array
 * @param idx index <0, arr->cnt)
 * @return NULL if idx is unused or out of range
 */
void *arr_get_item(struct arr *arr, size_t idx)
{
	if (idx >= arr->cnt) {
		return NULL;
	}

	return (char *)arr->ptr + (idx * arr->item_size);
}

/**
 * @brief Zero all array variables.
 *
 * @param arr array
 */
void arr_zero(struct arr *arr)
{
	arr->ptr = NULL;
	arr->cnt = 0;
	arr->item_size = 0;
}

/**
 * @brief Prepare array of some length.
 * @note Calloc(3) is required since NULL is flag for "unused index".
 *
 * @param arr array
 * @param cnt required number of items
 * @param item_size size every item takes
 * @return 0 on success, 1 on memory failure
 */
int arr_prep(struct arr *arr, size_t cnt, size_t item_size)
{
	assert(NULL == arr->ptr);
	assert(0 == arr->cnt);
	assert(0 == arr->item_size);

	assert(cnt > 0);
	assert(item_size > 0);

	arr->ptr = (struct arr *)calloc(cnt, item_size);
	if (NULL == arr->ptr) {
		return 1;
	}
	arr->cnt = cnt;
	arr->item_size = item_size;

	return 0;
}

/**
 * @brief Empty array variables.
 *
 * @param arr array
 */
void arr_empty(struct arr *arr)
{
	free(arr->ptr);

	arr_zero(arr);
}

/**
 * @brief Allocate arr structure.
 *
 * @param arr double pointer to array
 * @return 0 on success, 1 on memory failure
 */
int arr_alloc(struct arr **arr)
{
	assert(NULL == *arr);

	*arr = (struct arr *)malloc(sizeof(struct arr));
	if (NULL == *arr) {
		return 1;
	}

	arr_zero(*arr);

	return 0;
}

/**
 * @brief Deallocate arr structure.
 *
 * @param arr double pointer to array
 */
void arr_dealloc(struct arr **arr)
{
	assert(NULL != *arr);

	arr_empty(*arr);
	free(*arr);
	*arr = NULL;

}
