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
parse_options(int *argc, char ***argv,
              struct option long_options[], const char * last_opt = NULL);


struct ext_option {
  const char *name;    // see man getopt_long
  int         has_arg; // see man getopt_long
  int         val;     // see man getopt_long
  int         group;   // this setting is used to select group of options
  const char *desc;    // description, used in print_options()
};

Options
parse_options(int *argc, char ***argv,
              struct ext_option ext_options[],
              int mask,
              const char * last_opt = NULL);

void
print_options(struct ext_option ext_options[],
              int mask, std::ostream & s, bool pod=false);

#endif
