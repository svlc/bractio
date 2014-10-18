/**
 * @file debug.h
 * @brief
 * @athor Slavomir Vlcek
 * @copyright GPLv2
 */

#ifndef BRACT_DEBUG_H
#define BRACT_DEBUG_H

/* verbose guard, adds {file,function,...} info */
#define VGUARD(COND, ACTION)						\
	if ((COND)) {							\
		fprintf(stderr, "%s, %s\n", __FILE__, __func__);	\
		ACTION;							\
	}

#endif /* BRACT_DEBUG_H */
