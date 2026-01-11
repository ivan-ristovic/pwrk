#include "debug.h"
#include "opt.h"
#include "req.h"

int main(int argc, char *argv[])
{
    Options *opts = parse_args(argc, argv);
    DEBUG = opts->verbose;
    validate(opts);

    dbg("URL: %s", opts->url);
    dbg("Config path: %s\n", opts->cfg_path);

    req_init_global();
    
    req_get(opts->url);
    req_post(opts->url, "abc", 3);

    req_cleanup_global();

    return 0;
}
