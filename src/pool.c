#include "pool.h"
#include "req.h"
#include <unistd.h>

static void process_batch(const char *base_url, const Batch *batch) 
{
    // TODO create final URL
    const char *url = base_url;

    for (unsigned i = 0; i < batch->requests; i++) {
        switch (batch->type) {
            case REQUEST_TYPE_GET:
                req_get(url);               // TODO
                break;
            case REQUEST_TYPE_POST:
                req_post(url, "abc", 3);    // TODO
                break;
        }
        usleep(batch->delay_us);
    }
}

void pool_exec(const char *url, const Config *cfg)
{
    if (cfg == NULL) {
        // TODO
    }

    req_init_global();
    
    int i = 0;
    for (Batch *curr = cfg->batches; curr != NULL; curr = curr->next) {
        printf("[%2d/%2d] %4u req, %10zu bytes, %10ld delay (us), ep: %s\n",
                i, cfg->count, curr->requests, curr->alloc, curr->delay_us, curr->endpoint);
        process_batch(url, curr);
        i++;
    }

    req_cleanup_global();
}
