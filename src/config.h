#ifndef _CONFIG_H_
#define _CONFIG_H_

#include <stddef.h>
#include <stdint.h>
#include "measure.h"

typedef struct Config {
    const char* path;
    unsigned count;
    struct Batch *batches;
} Config;

typedef enum RequestType {
    REQUEST_TYPE_GET,
    REQUEST_TYPE_POST,
} RequestType;

typedef struct Batch {
    unsigned requests;
    RequestType type;
    size_t alloc;
    uint64_t delay_us;
    char *endpoint;
    struct Measurement *measurements;
    struct Batch *next;
} Batch;

Config* get_default_config();
Config* read_config(const char *path);
void free_config(Config *cfg);
const char *req_type_to_str(RequestType t);

#endif
