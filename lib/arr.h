/**
 * @file arr.h
 * @brief Generic array.
 * @athor Slavomir Vlcek
 * @copyright GPLv2
 */

#ifndef APM_ARR_H
#define APM_ARR_H

struct arr {

	void *ptr;
	size_t cnt;
	size_t item_size;

};


extern void arr_add_item(struct arr *arr, size_t idx, void *data);
extern void *arr_get_item(struct arr *arr, size_t idx);
extern void arr_zero(struct arr *arr);
extern int arr_prep(struct arr *arr, size_t cnt, size_t item_size);
extern void arr_empty(struct arr *arr);
extern int arr_alloc(struct arr **arr);
extern void arr_dealloc(struct arr **arr);


#endif /* APM_ARR_H */

