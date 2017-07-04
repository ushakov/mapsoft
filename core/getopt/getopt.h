#ifndef GETOPT_H
#define GETOPT_H

#include <getopt.h>
#include <vector>
#include <string>
#include "opts/opts.h"

// Parse cmdline options up to the first non-option argument.
// For the long_options structure see getopt_long (3).
// If option last_opt is found in the non-last position
// error occures.
// All options are returned as Opts object.
Opts
parse_options(int *argc, char ***argv,
              struct option long_options[], const char * last_opt = NULL);


// Same, but using ext_option structure which contains option descriptions
struct ext_option {
  const char *name;    // see man getopt_long
  int         has_arg; // see man getopt_long
  int         val;     // see man getopt_long
  int         group;   // this setting is used to select group of options
  const char *desc;    // description, used in print_options()
};
Opts
parse_options(int *argc, char ***argv,
              struct ext_option ext_options[],
              int mask,
              const char * last_opt = NULL);

// Print options in help/pod format.
// Mask is applied to the group element of the ext_option structure
void
print_options(struct ext_option ext_options[],
              int mask, std::ostream & s, bool pod=false);

//parse mixed options and non-option arguments (for simple programs)
Opts
parse_options_all(int *argc, char ***argv,
              struct ext_option ext_options[],
              int mask, std::vector<std::string> & non_opts);



#endif
