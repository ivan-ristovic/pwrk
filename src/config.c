#include "config.h"
#include "debug.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const char *REQ_TYPE_GET = "GET";
static const int REQ_TYPE_GET_SIZE = 3;
static const char *REQ_TYPE_POST = "POST";
static const int REQ_TYPE_POST_SIZE = 4;

static Batch* parse_batch(char *line, const char *delim)
{
    char* tok;
    
    // 1: request count
    tok = strtok(line, delim);
    if (tok == NULL || *tok == '\0')
        return NULL;
    unsigned requests = atoi(tok);
    if (requests == 0)
        return NULL;
    
    // 2: alloc size for each request
    tok = strtok(NULL, delim);
    if (tok == NULL || *tok == '\0')
        return NULL;
    size_t alloc = atoi(tok);
    if (alloc == 0)
        return NULL;

    // 3: delay between requests
    tok = strtok(NULL, delim);
    if (tok == NULL || *tok == '\0')
        return NULL;
    uint64_t delay_us = atoll(tok);
    if (delay_us == 0)
        return NULL;

    // 4: request type
    tok = strtok(NULL, delim);
    if (tok == NULL || *tok == '\0')
        return NULL;
    RequestType type;
    if (strncmp(tok, REQ_TYPE_GET, REQ_TYPE_GET_SIZE)) {
        type = REQUEST_TYPE_GET;
    } else if (strncmp(tok, REQ_TYPE_POST, REQ_TYPE_POST_SIZE)) {
        type = REQUEST_TYPE_POST;
    } else {
        return NULL;
    }

    // 5: endpoint
    tok = strtok(NULL, delim);
    if (tok == NULL || *tok == '\0')
        return NULL;
    char *endpoint = strdup(tok);
    panic_if(endpoint == NULL, "strdup() failed");

    // Create batch object
    Batch *b = malloc(sizeof(*b));
    b->requests = requests;
    b->alloc = alloc;
    b->delay_us = delay_us;
    b->type = type;
    b->endpoint = endpoint;
    b->next = NULL;
    return b;
}

Config* read_config(const char *path)
{
    dbg("Opening config file: %s", path);
    FILE* f = fopen(path, "r");
    panic_if(f == NULL, "fopen() failed");

    dbg("Allocating config");
    Config *cfg = malloc(sizeof(*cfg));
    panic_if(cfg == NULL, "malloc() failed");
    cfg->path = path;
    cfg->count = 0;
    cfg->batches = NULL;

    dbg("Reading config file: %s", path);
    const size_t BUFSIZE = 1024;
    char buf[BUFSIZE];
    Batch *last_batch = NULL;
    while (fgets(buf, BUFSIZE, f)) {
        char *tmp = strdup(buf);
        panic_if(tmp == NULL, "strdup() failed");
        Batch *next = parse_batch(tmp, ",\n");
        if (next) {
            if (last_batch == NULL) {
                cfg->batches = next;
            } else {
                last_batch->next = next;
            }
            cfg->count++;
            last_batch = next;
        } else {
            panic("Error reading config file, invalid line:\n%s", buf);
        }
        free(tmp);
    }

    fclose(f);

    return cfg;
}

void free_config(Config *cfg)
{
    dbg("Freeing config");
    if (cfg != NULL) {
        Batch* curr = cfg->batches;
        Batch* prev;
        while (curr != NULL) {
            prev = curr;
            curr = curr->next;
            free(prev->endpoint);
            free(prev);
        }
        free(cfg);
    }
}

const char *req_type_to_str(RequestType t) 
{
    switch (t) {
        case REQUEST_TYPE_GET:  return REQ_TYPE_GET;
        case REQUEST_TYPE_POST: return REQ_TYPE_POST;
        default:
            panic("Unknown request type");
    }
    return NULL;
}
