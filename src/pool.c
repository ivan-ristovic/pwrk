#include "pool.h"
#include "req.h"
#include <unistd.h>

void pool_exec(const Options *opts)
{
    req_init_global();
    
    for (int i = 0; i < 5; i++) {
        req_get(opts->url);
        req_post(opts->url, "abc", 3);
        sleep(1);
    }

    req_cleanup_global();
}
