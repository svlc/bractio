/**
 * @file debug.h
 * @brief
 * @athor Slavomir Vlcek
 * @copyright GPLv2
 */

#ifndef APM_DEBUG_H
#define APM_DEBUG_H


/* verbose guard, adds {file,function,...} info */
#define VGUARD(COND, ACTION)						\
	if ((COND)) {							\
		fprintf(stderr, "%s, %s\n", __FILE__, __func__);	\
		ACTION;							\
	}


/* return "RET" if given "COND" is true */
#define GUARD(COND, ACTION)			\
	if ((COND)) {				\
		ACTION;				\
	}


/**
 * @brief 
 */
#define MALLOC(PTR, CONV, SIZE, ACTION)		\
	do {					\
		PTR = CONV malloc(SIZE);	\
		if (NULL == PTR) {		\
			ACTION;			\
		}				\
	} while (0)				\


/**
 * @brief
 */
#define CALLOC(PTR, CONV, SIZE, ACTION)		\
	do {					\
		PTR = CONV calloc(1, SIZE);	\
		if (NULL == PTR) {		\
			ACTION;			\
		}				\
	} while (0)				\


/**
 * @brief 
 */
#define REALLOC(NEWP, OLDP, CONV, SIZE, ACTION)		\
	do {						\
		NEWP = CONV realloc(OLDP, SIZE);	\
		if (NULL == NEWP) {			\
			ACTION;				\
		}					\
	} while (0)					\

#endif /* APM_DEBUG_H */
