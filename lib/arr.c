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
#include <assert.h>
#include <string.h>

#include "arr.h"

/**
 * @brief Add some data at specified index.
 * 
 * @param a array
 * @param idx index
 * @param data
 */
void arr_add(struct arr *a, size_t idx, void *data)
{
	assert(idx < a->len);
	memcpy((char *)a->ptr + idx * a->item_size, data, a->item_size);
}

/**
 * @brief Get the pointer to item.
 *
 * @param a array
 * @param idx index <0, arr->len)
 * @return NULL if idx is unused or out of range
 */
void *arr_get(struct arr *a, size_t idx)
{
	assert(idx < a->len);
	return (char *)a->ptr + (idx * a->item_size);
}

/**
 * @brief Allocate "arr" struct.
 */
struct arr *arr_alloc(size_t len, size_t item_size)
{
	struct arr *a;
	assert(len > 0);

	a = (struct arr *)malloc(sizeof(*a));
	if (!a)
		goto err;
	a->ptr = malloc(len * item_size);
	if (!a->ptr)
		goto err;
	a->len = len;
	a->item_size = item_size;
	return a;
err:
	arr_dealloc(a);
	return NULL;
}

/**
 * @brief Deallocate "arr" struct.
 */
void arr_dealloc(void *p)
{
	struct arr *a = (struct arr *)p;

	if (!a)
		return;

	free(a->ptr);
	free(a);	
}
