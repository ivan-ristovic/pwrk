#ifndef _DEBUG_H_
#define _DEBUG_H_

#include <assert.h>
#include <error.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

extern bool DEBUG;

#define dbg(fmt, ...) \
    do { \
        if (DEBUG) fprintf(stderr, "%s:%d:%s(): " fmt "\n", \
                           __FILE__, __LINE__, __func__, ##__VA_ARGS__); \
    } while (0)

#define panic_if(cond, fmt, ...) \
    do { \
        if (cond) error(EXIT_FAILURE, errno, "%s:%d:%s(): " fmt "\n", \
                        __FILE__, __LINE__, __func__, ##__VA_ARGS__); \
    } while (0)

#define panic(...) panic_if(1, __VA_ARGS__)

#endif
