#include "debug.h"
#include "opt.h"
#include "req.h"

int main(int argc, char *argv[])
{
    Options *opts = parse_args(argc, argv);
    DEBUG = opts->verbose;
    validate(opts);

    dbg("Config path: %s\n", opts->cfg_path);

    req_init();
    
    req_get("http://localhost:8000");

    req_cleanup();

    return 0;
}
