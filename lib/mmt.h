/**
 *@file mmt.h
 *@brief
 *@athor Slavomir Vlcek
 *@copyright GPLv2
 */

#ifndef APM_MMT_H
#define APM_MMT_H

/**
 *@brief time moment info
 */
typedef struct mmt_t {

	bool valid;

	unsigned hour;
	unsigned min;
	unsigned sec;
	unsigned ms;

	/* in milliseconds */
	unsigned long total_ms;
} mmt_t;


extern void moment_zero(mmt_t *mmt);
extern double moment_min(mmt_t *mmt);
extern void moment_inc(mmt_t *mmt, unsigned long time_inc);

#endif /* APM_MMT_H */
