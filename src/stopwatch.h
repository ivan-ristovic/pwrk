#ifndef _STOPWATCH_H_
#define _STOPWATCH_H_

#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <sys/timeb.h>

static uint64_t ts_ns()
{
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return (uint64_t)ts.tv_sec * 1000000000LL + (uint64_t)ts.tv_nsec;
}

#define STOPWATCH_MEASURE(result, block)   \
     do {                                  \
          uint64_t start = ts_ns();        \
          do block while(0);               \
          uint64_t end = ts_ns();          \
          *result = end - start;           \
     } while(0);

#endif
