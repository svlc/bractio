/**
 *@file attr.c
 *@brief 
 *@athor Slavomir Vlcek
 *@copyright GPLv2
 */

#include "rapm.h"
#include "debug.h"


/**
 * @brief
 *
 * @param path file path
 */
void apm_wc3_attr_setfilepath(apm_wc3_attr_t *attr, char *path)
{
	attr->path = path;
}


/**
 * @brief 
 */
void apm_wc3_attr_settask(apm_wc3_attr_t *attr, task_t task)
{
	attr->task = task;
}


/**
 * @brief 
 */
void apm_wc3_attr_init(apm_wc3_attr_t *attr)
{
	attr->path = NULL;

	attr->task = APM_TASK_NOTHING;
}


/**
 * @brief currently NOOP
 */
void apm_wc3_attr_deinit(apm_wc3_attr_t *attr)
{
	attr = attr;

	return;
}
