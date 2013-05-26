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
	size_t len;
	size_t item_size;
};

void arr_add(struct arr *a, size_t idx, void *data);
void *arr_get(struct arr *a, size_t idx);
struct arr *arr_alloc(size_t len, size_t item_size);
void arr_dealloc(void *p);

#endif /* APM_ARR_H */

