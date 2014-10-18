/**
 *@file attr.c
 *@brief 
 *@athor Slavomir Vlcek
 *@copyright GPLv2
 */

#include "bract.h"
#include "debug.h"

/**
 * @brief
 *
 * @param path file path
 */
void bract_wc3_attr_setfilepath(bract_wc3_attr_t *attr, char *path)
{
	attr->path = path;
}

/**
 * @brief 
 */
void bract_wc3_attr_settask(bract_wc3_attr_t *attr, task_t task)
{
	attr->task = task;
}

/**
 * @brief 
 */
void bract_wc3_attr_init(bract_wc3_attr_t *attr)
{
	attr->path = NULL;
	attr->task = BRACT_TASK_NOTHING;
}

/**
 * @brief currently NOOP
 */
void bract_wc3_attr_deinit(bract_wc3_attr_t *attr)
{
	attr = attr;
	return;
}
