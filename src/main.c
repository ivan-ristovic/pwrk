#include "debug.h"
#include "config.h"
#include "opt.h"
#include "pool.h"

int main(int argc, char *argv[])
{
    Options *opts = parse_args(argc, argv);
    DEBUG = opts->verbose;
    validate(opts);

    Config *cfg = NULL;
    if (opts->cfg_path) {
        dbg("Config path: %s\n", opts->cfg_path);
        cfg = read_config(opts->cfg_path);
        dbg("Config file contains %u batches:", cfg->count);
        size_t i = 0;
        for (Batch *b = cfg->batches; b != NULL; b = b->next) {
            dbg("%02zu: %3u req, %10zu bytes, %10ld, delay (us)",
                i, b->requests, b->alloc, b->delay_us);
            i++;
        }
        panic_if(cfg->count != i, "Batch count mismatch");
    }

    dbg("URL: %s", opts->url);
    pool_exec(opts->url);

    free_config(cfg);
    return 0;
}
