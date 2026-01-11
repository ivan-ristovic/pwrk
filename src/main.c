#include "debug.h"
#include "opt.h"
#include "pool.h"

int main(int argc, char *argv[])
{
    Options *opts = parse_args(argc, argv);
    DEBUG = opts->verbose;
    validate(opts);

    dbg("URL: %s", opts->url);
    dbg("Config path: %s\n", opts->cfg_path);

    pool_exec(opts);

    return 0;
}
