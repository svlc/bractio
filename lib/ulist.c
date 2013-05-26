/**
 * @file ulist.c
 * @brief unrolled singly-linked list
 * @athor Slavomir Vlcek
 * @copyright GPLv2
 *
 * Partial (some unneeded operations -- e.g. deleting -- are omitted)
 * implementation of unrolled (singly-linked) list data structure.
 */

#include <stdlib.h>		/* NULL, {m,c}alloc(3), free(3) */
#include <errno.h>
#include <assert.h>

#include "ulist.h"

struct record {
	void **arr;
	/* items stored */
	size_t item_cnt;
	struct record *next;
};

/**
 * @brief
 */
void ulist_dealloc(struct ulist *u)
{
	struct record *cur = u->head;
	
	if (!u)
		return;

	while (cur) {
		struct record *tmp = cur;

		if (u->free_data_fn) {
			void **data;

			for (data = cur->arr;  cur->item_cnt--;  ++data) {
				u->free_data_fn(*data);
				--u->cnt;
			}
		} else {
			u->cnt -= cur->item_cnt;
		}
		--u->rec_cnt;
		cur = cur->next;
		free(tmp);
	}
	free(u);
}

/**
 * @brief
 */
struct ulist *ulist_alloc(size_t arr_size, ulist_free_data_fn_t free_data_fn)
{
	struct ulist *u;
	assert(arr_size > 0);

	u = (struct ulist *)calloc(1, sizeof(*u));
	if (!u) {
		goto out;
	}
	u->arr_size = arr_size;
	u->free_data_fn = free_data_fn;
out:
	return u;
}

/**
 * @brief
 */
static struct record *get_record(struct record *cur_rec,
				 size_t cur_no, size_t dest_no)
{
	while (cur_no++ < dest_no) {
		cur_rec = cur_rec->next;
	}
	return cur_rec;
}

/**
 * @brief
 */
void *ulist_get(struct ulist *u, size_t idx)
{
	div_t d;
	void *data;
	size_t quot;

	if (idx >= u->cnt) {
		data = NULL;
		goto out;
	}
	d = div(idx, u->arr_size);
	quot = d.quot;

	if (u->last_rec_no != quot) {
		/* if it maps to the last record */
		if (quot == u->rec_cnt - 1) {
			u->last_rec = u->tail;
		} else if (u->last_rec_no < quot) {
			u->last_rec = get_record(u->last_rec,
						 u->last_rec_no, quot);
		} else if (u->last_rec_no > quot) {
			u->last_rec = get_record(u->head, 0, quot);
		}
		u->last_rec_no = quot;
	}
	data = u->last_rec->arr[(size_t)d.rem];
out:
	return data;
}

/**
 * @brief
 */
int ulist_append(struct ulist *u, void *data)
{
	div_t d;
	int ret = 0;

	d = div(u->cnt, u->arr_size);
	if (0 == d.rem) {
		struct record *r;

		/* allocate record and it's array at the same time */
		r = (struct record *)calloc(1, sizeof(*r)
					    + u->arr_size * sizeof(void *));
		if (!r) {
			ret = -1;
			goto out;
		}
		/* assign address that is right behind "record" struct */
		r->arr = (void **)((char *)r + sizeof(*r));
		/* if this is the first allocated record */
		if (!u->head) {
			u->head = r;
		} else {
			u->tail->next = r;
		}
		u->tail = u->last_rec = r;
		u->last_rec_no = d.quot;
		++u->rec_cnt;
	}
	u->tail->arr[d.rem] = data;
	++u->tail->item_cnt;
	++u->cnt;
out:
	return ret;
}
