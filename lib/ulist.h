/**
 * @file list.h
 * @brief unrolled singly-linked list
 * @athor Slavomir Vlcek
 * @copyright GPLv2
 */

#ifndef APM_ULIST_H
#define APM_ULIST_H

typedef void (*ulist_free_data_fn_t)(void *p);

/**
 * @brief
 */
struct ulist {
	/* total number of items currently stored */
	size_t cnt;
	size_t rec_cnt;

	/* length of array needed for every record */
	size_t arr_size;

	struct record *head;
	struct record *tail;

	/* last retrived record, zero-based numbering */
	size_t last_rec_no;
	struct record *last_rec;

	ulist_free_data_fn_t free_data_fn;
};

void ulist_dealloc(struct ulist *u);
struct ulist *ulist_alloc(size_t arr_size, ulist_free_data_fn_t free_data_fn);
void *ulist_get(struct ulist *u, size_t idx);
int ulist_append(struct ulist *u, void *data);

#endif /* APM_ULIST_H */
