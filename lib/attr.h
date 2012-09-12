/**
 *@file attr.h
 *@brief
 *@athor Slavomir Vlcek
 *@copyright GPLv2
 */

#ifndef APM_ATTR_H
#define APM_ATTR_H


/**
 * @brief task bitmap: what data should be collected out of replay
 */
typedef enum task_t {

	/* default request state */
	APM_TASK_NOTHING = 0,

	/* basic data (version, build, replay length in ms, ...) */
	APM_TASK_BASIC = (1 << 0),

	APM_TASK_ADDTL = (1 << 1),

	/* actions per minute */
	APM_TASK_APM = (1 << 2)

} task_t;


/**
 * @brief 
 */
typedef struct apm_wc3_attr_t {

	char *path;

	task_t task;

} apm_wc3_attr_t;


extern void apm_wc3_attr_setfilepath(apm_wc3_attr_t *attr, char *path);
extern void apm_wc3_attr_settask(apm_wc3_attr_t *attr, task_t task);
extern void apm_wc3_attr_init(apm_wc3_attr_t *attr);
extern void apm_wc3_attr_deinit(apm_wc3_attr_t *attr);

#endif /* APM_ATTR_H */
