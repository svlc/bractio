/**
 * @file buffer.h
 * @brief 
 * @athor Slavomir Vlcek
 * @copyright GPLv2
 */

#ifndef BRACT_BUFFER_H
#define BRACT_BUFFER_H

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

#endif /* BRACT_BUFFER_H */
