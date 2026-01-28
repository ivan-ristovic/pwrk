#include "debug.h"
#include "pool.h"
#include "stopwatch.h"
#include "req.h"
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

static void process_batch(const char *base_url, Batch *batch) 
{
    // Make final URL
    const unsigned long base_url_size = strlen(base_url);
    const unsigned long endpoint_size = strlen(batch->endpoint);
    char *url = malloc(base_url_size + endpoint_size + 1);
    panic_if(url == NULL, "malloc() failed");
    memcpy(url, base_url, base_url_size);
    memcpy(url + base_url_size, batch->endpoint, endpoint_size);
    url[base_url_size + endpoint_size + 1] = '\0';
    dbg("URL: %s", url);

    // Run batch
    for (unsigned i = 0; i < batch->requests; i++) {

        // Measure latency to adjust delay
        uint64_t adjust = ts_ns();

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
        batch->measurements[i].status = code;

        // Calculate delay
        adjust = ts_ns() - adjust;
        dbg("delay : %ld", batch->delay_ns);
        dbg("adjust: %ld", adjust);
        uint64_t delay = 0;
        if (batch->delay_ns > adjust) {
            delay = batch->delay_ns - adjust;
        }

        // Delay until next request
        if (delay > 0) {
            dbg("sleep : %ld", delay);
            struct timespec dt = { 
                delay / 1000000000UL, 
                delay % 1000000000UL 
            };
            nanosleep(&dt, &dt);
        }
    }

    // Cleanup
    free(url);
}

void pool_exec(const char *url, const Config *cfg)
{
    req_init_global();
    
    int i = 1;
    for (Batch *curr = cfg->batches; curr != NULL; curr = curr->next) {
        printf("[%2d/%2d] %4u req, %10zu bytes, %10ldns delay, %4s %s\n",
                i, cfg->count, curr->requests, curr->alloc, curr->delay_ns, req_type_to_str(curr->type), curr->endpoint);
        process_batch(url, curr);
        i++;
    }

    req_cleanup_global();
}
