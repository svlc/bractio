/**
 *@file attr.h
 *@brief
 *@athor Slavomir Vlcek
 *@copyright GPLv2
 */

#ifndef BRACT_ATTR_H
#define BRACT_ATTR_H

/**
 * @brief task bitmap: what data should be collected out of replay
 */
typedef enum task_t {
	/* default request state */
	BRACT_TASK_NOTHING = 0,

	/* basic data (version, build, replay length in ms, ...) */
	BRACT_TASK_BASIC = (1 << 0),

	BRACT_TASK_ADDTL = (1 << 1),

	/* actions per minute */
	BRACT_TASK_APM = (1 << 2)
} task_t;

/**
 * @brief 
 */
typedef struct bract_wc3_attr_t {
	char *path;

	task_t task;
} bract_wc3_attr_t;

extern void bract_wc3_attr_setfilepath(bract_wc3_attr_t *attr, char *path);
extern void bract_wc3_attr_settask(bract_wc3_attr_t *attr, task_t task);
extern void bract_wc3_attr_init(bract_wc3_attr_t *attr);
extern void bract_wc3_attr_deinit(bract_wc3_attr_t *attr);

#endif /* BRACT_ATTR_H */
