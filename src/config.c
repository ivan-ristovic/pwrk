#include "config.h"
#include "debug.h"
#include <errno.h>
#include <math.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const char *REQ_TYPE_GET = "GET";
static const int REQ_TYPE_GET_SIZE = 3;
static const char *REQ_TYPE_POST = "POST";
static const int REQ_TYPE_POST_SIZE = 4;

static void sort_and_print_hist(uint64_t *values, size_t n, const char *prefix);

static void *heap_alloc(size_t nbytes)
{
    void *mem = malloc(nbytes);
    panic_if(mem == NULL, "malloc() failed");
    return mem;
}

static Batch* parse_batch(char *line, const char *delim)
{
    char* tok;
    
    // 1: request count
    tok = strtok(line, delim);
    if (tok == NULL || *tok == '\0')
        return NULL;
    int errno_old = errno;
    uint64_t requests = strtoull(tok, NULL, 10);
    if (requests == 0 || (errno != errno_old && (errno == EINVAL || errno == ERANGE)))
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
    if (strncmp(tok, REQ_TYPE_GET, REQ_TYPE_GET_SIZE) == 0) {
        type = REQUEST_TYPE_GET;
    } else if (strncmp(tok, REQ_TYPE_POST, REQ_TYPE_POST_SIZE) == 0) {
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
    Batch *b = heap_alloc(sizeof(*b));
    b->requests = requests;
    b->alloc = alloc;
    b->delay_us = delay_us;
    b->type = type;
    b->endpoint = endpoint;
    b->measurements = calloc(requests, sizeof(*(b->measurements)));
    b->next = NULL;

    panic_if(b->measurements == NULL, "calloc() failed");

    return b;
}

Config* read_config(const char *path)
{
    dbg("Opening config file: %s", path);
    FILE* f = fopen(path, "r");
    panic_if(f == NULL, "fopen() failed");

    dbg("Allocating config");
    Config *cfg = heap_alloc(sizeof(*cfg));
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

Config* get_default_config()
{
    Batch *b = heap_alloc(sizeof(*b));
    b->alloc = 1024;
    b->delay_us = 1000000;
    b->endpoint = "/";
    b->requests = 1000;
    b->measurements = calloc(b->requests, sizeof(*(b->measurements)));
    b->next = NULL;
    b->type = REQUEST_TYPE_GET;
    panic_if(b->measurements == NULL, "calloc() failed");

    Config *cfg = heap_alloc(sizeof(*cfg));
    cfg->batches = b;
    cfg->count = 1;
    cfg->path = NULL;

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

void print_results(const Config *cfg)
{
    printf("\n--- Results ---\n");

    size_t total = 0;
    for (Batch *curr = cfg->batches; curr != NULL; curr = curr->next) {
        total += curr->requests;
    }

    uint64_t *latencies = malloc(total * sizeof(*latencies));
    panic_if(latencies == NULL, "malloc() failed");

    size_t n = 0;
    unsigned int batch_id = 1;
    uint64_t total_success = 0;
    uint64_t total_failed = 0;
    
    for (Batch *curr = cfg->batches; curr != NULL; curr = curr->next) {
        uint64_t failed = 0;
        size_t begin = n;
        for (uint64_t i = 0; i < curr->requests; i++) {
            if (curr->measurements[i].status == 200) {
                uint64_t v = curr->measurements[i].latency_ns;
                latencies[n] = v;
                n++;
            } else {
                failed++;
            }
        }
        size_t end = n;
        size_t nb = end - begin;

        char prefix[8];
        snprintf(prefix, 8, "[%2u/%2u]", batch_id, cfg->count);

        printf("%s samples: %zu out of %u (%zu errored)\n", 
               prefix, nb, curr->requests, failed);

        if (nb > 0) {
            sort_and_print_hist(latencies + begin, nb, prefix);
        }

        total_success += curr->requests - failed;
        total_failed += failed;
        batch_id++;
    }

    printf("[total] samples: %zu out of %zu (%zu errored)\n", 
           total_success, total_success + total_failed, total_failed);

    if (total_success > 0) {
        sort_and_print_hist(latencies, n, "[total]");
    }

    free(latencies);
}

static int cmp(const void *v1, const void *v2) 
{
    uint64_t i1 = *((uint64_t *)v1);
    uint64_t i2 = *((uint64_t *)v2);
    return i1 - i2;    
}

static double percentile(const uint64_t *vs, size_t n, double p) {
    if (p <= 0.0) return vs[0];
    if (p >= 100.0) return vs[n - 1];

    double r = (p / 100.0) * (n - 1);
    uint64_t lo = (uint64_t)floor(r);
    uint64_t hi = (uint64_t)ceil(r);

    double frac = r - lo;
    return vs[lo] + frac * (vs[hi] - vs[lo]);
}

static void sort_and_print_hist(uint64_t *values, size_t n, const char *prefix)
{
    qsort(values, n, sizeof(*values), &cmp);

    uint64_t min = values[0];
    uint64_t max = values[n - 1];
    printf("%s min: %zuns max: %zuns\n", prefix, min, max);

    double ps[] = { 25.0, 50.0, 75.0, 90.0, 99.9, 99.99, 99.999 };
    int nps = sizeof(ps) / sizeof(ps[0]);
    for (int i = 0; i < nps; i++) {
        double p = ps[i];
        printf("%s p%2.3f: %.1f\n", prefix, p, percentile(values, n, p));
    }
}
