#include "pool.h"
#include "req.h"

static void process_batch(const char *url, const Batch *batch) 
{
    req_get(url);
    req_post(url, "abc", 3);
}

void pool_exec(const char *url, const Config *cfg)
{
    req_init_global();
    
    int i = 0;
    for (Batch *curr = cfg->batches; curr != NULL; curr = curr->next) {
        printf("[%2d/%2d] %3u req, %10zu bytes, %10ld delay (us), endpoint: %s",
                i, cfg->count, curr->requests, curr->alloc, curr->delay_us, curr->endpoint);
        process_batch(url, curr);
        i++;
    }

    req_cleanup_global();
}
