#ifndef _CONFIG_H_
#define _CONFIG_H_

#include <stddef.h>
#include <stdint.h>

typedef struct {
    const char* path;
    unsigned count;
    struct Batch *batches;
} Config;

typedef struct Batch {
    unsigned requests;
    size_t alloc;
    uint64_t delay_us;
    struct Batch *next;
} Batch;

Config* read_config(const char *path);
void free_config(Config *cfg);

#endif
