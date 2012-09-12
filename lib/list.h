/**
 * @file list.h
 * @brief singly-linked list
 * @athor Slavomir Vlcek
 * @copyright GPLv2
 */

#ifndef APM_LIST_H
#define APM_LIST_H


typedef void (*list_free_data_fn_t)(void *data);


/**
 * @brief 
 */
typedef struct item_t {	

	void *data;

	struct item_t *next;

} item_t;


/**
 * @brief 
 */
typedef struct list_t {

	struct item_t *head;
	struct item_t *tail;

	struct item_t *curr;

	unsigned curr_idx;

	/* item cnt */
	unsigned cnt;

	/* data deallocation function */
	list_free_data_fn_t list_free_data_fn;
	

} list_t;

extern int list_add_item(list_t *ls, void *data);
extern void list_zero(list_t *ls);
extern void list_prep(list_t *ls, list_free_data_fn_t fn);
extern void list_empty(list_t *ls);
extern int list_alloc(list_t **ls);
extern void list_dealloc(list_t **ls);

#endif /* APM_LIST_H */
