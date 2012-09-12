/**
 * @file buffer.h
 * @brief 
 * @athor Slavomir Vlcek
 * @copyright GPLv2
 */

#ifndef APM_BUFFER_H
#define APM_BUFFER_H

typedef struct buff_t {

	char *arr;

	char *pos;

	size_t len;

	char *lim;

} buff_t;

int buff_prep(buff_t *buff, const size_t len);
int buff_prep(buff_t *buff, const size_t len);
void buff_empty(buff_t *buff);
void buff_pos_reset(buff_t *buff);

#endif /* APM_BUFFER_H */
