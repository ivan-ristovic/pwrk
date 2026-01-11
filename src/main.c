#include "debug.h"
#include "opt.h"

int main(int argc, char *argv[])
{
    Options *opts = parse_args(argc, argv);
    DEBUG = opts->verbose;
    validate(opts);

    printf("Config: %s\n", opts->cfg_path);

    return 0;
}
