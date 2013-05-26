/**
 *@file str.h
 *@brief
 *@athor Slavomir Vlcek
 *@copyright GPLv2
 */

#ifndef APM_STR_H
#define APM_STR_H

/**
 * @brief various data types
 */
typedef enum datatype_t {
	APM_UCHAR,
	APM_UINT,
	APM_ULONG
} datatype_t;

/**
 *@brief auxiliary
 *
 *@note none of members can be 'const'
 */
typedef struct aux_t {
	void *dest;
	datatype_t type;
	int size;
} aux_t;

extern bool pos_eq_lim(const char *pos, const char *lim);
extern int safe_pos_fw(char **pos, char *lim, const size_t size);
extern void unsafe_pos_bw(char **pos, const size_t size);
extern int safe_mem_read(char **pos, const char *lim,
			 aux_t *aux_arr, const size_t cnt);

#endif /* APM_STR_H */
