/**
 *@file debug.h
 *@brief
 *@athor Slavomir Vlcek
 */

#ifndef APM_DEBUG_H
#define APM_DEBUG_H


/*
 * VGUARD() uses variadic macros:
 * http://gcc.gnu.org/onlinedocs/cpp/Variadic-Macros.html
 */

/* returns string according to "errno" value */
#define ECHO_ERRNO() (errno ? strerror(errno) : "" )

/* VERBOSE GUARD
 * @brief if given "COND" is true:
 * error info and reason is printed,
 * "errno" variable is reset,
 * "RET" is returned */
#define VGUARD(COND, RET, ...)                                          \
        if((COND)) {                                                    \
                fprintf(stderr, "ERROR in %s, %s(),\nerrno: \"%s\", ",  \
                        __FILE__, __func__, ECHO_ERRNO());              \
                fprintf(stderr, ##__VA_ARGS__);                         \
                errno = 0;                                              \
                return RET;                                             \
        }


/* return "RET" if given "COND" is true */
#define GUARD(COND, RET)                        \
        if((COND)) {                            \
                return RET;                     \
        }


#endif /* APM_DEBUG_H */
