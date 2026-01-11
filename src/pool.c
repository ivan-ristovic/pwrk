#include "pool.h"
#include "req.h"
#include <unistd.h>

void pool_exec(const char *url)
{
    req_init_global();
    
    sleep(1);
    for (int i = 0; i < 5; i++) {
        req_get(url);
        req_post(url, "abc", 3);
        sleep(1);
    }

    req_cleanup_global();
}
