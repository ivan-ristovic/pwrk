#include "debug.h"
#include "pool.h"
#include "req.h"
#include <time.h>
#include <unistd.h>

static void process_batch(const char *base_url, Batch *batch) 
{
    // TODO create final URL
    const char *url = base_url;

    time_t delay_s = batch->delay_us / 1000000UL;
    time_t delay_ns = (batch->delay_us % 1000000UL) * 1000UL;
    for (unsigned i = 0; i < batch->requests; i++) {
        int code = 0;
        switch (batch->type) {
            case REQUEST_TYPE_GET:
                code = req_get(url, &batch->measurements[i]);
                break;
            case REQUEST_TYPE_POST:
                // TODO
                code = req_post(url, "abc", 3, &batch->measurements[i]);
                break;
        }

        // TODO check code

        struct timespec dt = { delay_s, delay_ns };
        nanosleep(&dt, &dt);
    }
}

void pool_exec(const char *url, const Config *cfg)
{
    req_init_global();
    
    int i = 1;
    for (Batch *curr = cfg->batches; curr != NULL; curr = curr->next) {
        printf("[%2d/%2d] %4u req, %10zu bytes, %10ldus delay, %s %s\n",
                i, cfg->count, curr->requests, curr->alloc, curr->delay_us, req_type_to_str(curr->type), curr->endpoint);
        process_batch(url, curr);
        i++;
    }

    req_cleanup_global();
}
