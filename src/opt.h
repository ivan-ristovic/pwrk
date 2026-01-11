#ifndef _OPT_H_
#define _OPT_H_

#include <argp.h>
#include <argz.h>
#include <regex.h>
#include <stdbool.h>
#include <unistd.h>

extern const unsigned MODE_ALL;
extern const unsigned MODE_WHEN;
extern const unsigned MODE_UNTIL;

typedef struct {
    bool verbose;
    char* url;
    char* cfg_path;
} Options;

Options* parse_args(int argc, char **argv);
void validate(Options *opts);

#endif
