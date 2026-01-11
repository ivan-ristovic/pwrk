#include "opt.h"

const unsigned MODE_ALL = 1;
const unsigned MODE_WHEN = 1 << 1;
const unsigned MODE_UNTIL = 1 << 2;

static Options _opts;

const char *argp_program_version = "pwrk 0.0.1";
const char *argp_program_bug_address = "<contact@ristovic.net>";
static char doc[] = "Programmable workload generator";
static char args_doc[] = "URL";
static struct argp_option options[] = { 
    { "verbose", 'v', 0, 0, "Enable debug logging", 0},
    { "config", 'c', "CONFIG_FILE", 0, "Config file path", 0},
    { 0 } 
};

static error_t parse_opt(int key, char *arg, struct argp_state *state) 
{
    Options *opts = state->input;

    switch (key)
    {
        case 'v':
            opts->verbose = true;
            break;

        case 'c':
            opts->cfg_path = arg;
            break;

        case ARGP_KEY_INIT:
            opts->verbose = false;
            break;

        case ARGP_KEY_ARG:
            opts->url = arg;
            break;

        case ARGP_KEY_END:
            if (state->arg_num < 1)
                argp_usage(state);
            break;

        default:
            return ARGP_ERR_UNKNOWN;
    }

    return 0;
}

static struct argp argp = { options, parse_opt, args_doc, doc, 0, 0, 0 };

Options* parse_args(int argc, char **argv)
{
    argp_parse(&argp, argc, argv, 0, 0, &_opts);
    return &_opts;
}

void validate(Options *opts)
{
    (void) opts;    
}

