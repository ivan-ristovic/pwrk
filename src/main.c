#include "debug.h"
#include "config.h"
#include "opt.h"
#include "pool.h"

int main(int argc, char *argv[])
{
    Options *opts = parse_args(argc, argv);
    DEBUG = opts->verbose;
    validate(opts);

    if (opts->cfg_path) {
        dbg("Config path: %s\n", opts->cfg_path);
        read_config(opts->cfg_path);
    }

    dbg("URL: %s", opts->url);
    pool_exec(opts->url);

    return 0;
}
