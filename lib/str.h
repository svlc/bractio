/**
 *@file str.h
 *@brief
 *@athor Slavomir Vlcek
 *@copyright GPLv2
 */

#ifndef APM_STR_H
#define APM_STR_H

extern void _rev_char_seq(char *const seq, const int len);
extern bool _position_is_out_of_range(const char *curr_pos, const char *limit,
				      const int size);
extern int _safe_str_len(const char *start, const char *limit);

#endif /* APM_STR_H */
