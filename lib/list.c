/**
 * @file list.c
 * @brief singly-linked list
 * @athor Slavomir Vlcek
 * @copyright GPLv2
 */

#include <stdlib.h>		/* NULL, {m,c}alloc(3), free(3) */
#include <errno.h>
#include <stdbool.h>
#include <assert.h>

#include "list.h"


/**
 * @brief Get new allocated and initialized list item.
 * @note Internal function.
 *
 * @param data
 * @return NULL on failure, allocated item_t on success
 */
static item_t *_list_alloc_item(void *data)
{
	item_t *item;

	item = (item_t *)malloc(sizeof(item_t));
	if (NULL == item) {
		return NULL;
	}
	item->data = data;
	item->next = NULL;

	return item;
}

/**
 * @brief Add item to end of list.
 *
 * @param ls list
 * @param data
 * @return 0 on success, 1 on failure
 */
int list_add_item(list_t *ls, void *data)
{
	item_t *item = _list_alloc_item(data);
	if (NULL == item) {
		return 1;
	}

	/* if list is empty */
	if (0 == ls->cnt) {
		ls->head = ls->tail = item;
	} else {
		ls->tail->next = item;

		/* there is new tail item */
		ls->tail = item;
	}
	++ls->cnt;

	return 0;
}

/**
 * @brief Zero list variables.
 *
 * @param ls list
 */
void list_zero(list_t *ls)
{
	ls->head = NULL;
	ls->tail = NULL;
	ls->curr = NULL;
	ls->curr_idx = 0;
	ls->cnt = 0;

	ls->list_free_data_fn = NULL;
}

/**
 * @brief Prepare empty list.
 *
 * @param fn function to pass pointer to "free(3)" or something else
 */
void list_prep(list_t *ls, list_free_data_fn_t fn)
{
	list_zero(ls);

	/* the most basic deallocation way is to pass data to free(3) */
	ls->list_free_data_fn = fn;
}

/**
 * @brief Set list to empty state.
 *
 * @oaran ls list
 */
void list_empty(list_t *ls)
{
	item_t *aux = ls->head;
	item_t *tmp;

	while (aux) {
		tmp = aux;

		aux = aux->next;

		ls->list_free_data_fn(tmp->data);
		free(tmp);

		--ls->cnt;
	}

	/* now item count must be 0 */
	assert(0 == ls->cnt);

	list_zero(ls);
}

/**
 * @brief Allocate list with no items.
 * 
 * @param ls double pointer to list
 * @return 0 on success, 1 on memory failure
 */
int list_alloc(list_t **ls)
{
	assert(NULL == *ls);

	*ls = (list_t *)malloc(sizeof(list_t));
	if (NULL == *ls) {
		return 1;
	}

	list_zero(*ls);

	return 0;
}

/**
 * @brief Deallocate list_t structure.
 *
 * @param ls double pointer to list
 */
void list_dealloc(list_t **ls)
{
	assert(NULL != *ls);

	list_empty(*ls);
	free(*ls);
	*ls = NULL;
}
