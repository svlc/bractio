/**
 * @file arr.h
 * @brief Generic array.
 * @athor Slavomir Vlcek
 * @copyright GPLv2
 */

#ifndef APM_ARR_H
#define APM_ARR_H

typedef struct arr_t {

	void *ptr;
	size_t cnt;
	size_t item_size;

} arr_t;


extern void arr_add_item(arr_t *arr, size_t idx, void *data);
extern void *arr_get_item(arr_t *arr, size_t idx);
extern void arr_zero(arr_t *arr);
extern int arr_prep(arr_t *arr, size_t cnt, size_t item_size);
extern void arr_empty(arr_t *arr);
extern int arr_alloc(arr_t **arr);
extern void arr_dealloc(arr_t **arr);


#endif /* APM_ARR_H */

