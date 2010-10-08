#ifndef M_GETOPT_H
#define M_GETOPT_H

#include <getopt.h>
#include "options.h"

// Parse cmdline options up to the first non-option argument.
// For the long_options structure see getopt_long (3).
// If option last_opt is found in the non-last position
// error occures.
// All options are returned as Options object.
Options
parse_options(int argc, char **argv,
              struct option long_options[], const char * last_opt = NULL);

#endif
