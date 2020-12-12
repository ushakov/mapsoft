#include <iostream>
#include "spirit_utils.h"

using namespace boost::spirit::classic;

/** function for parsing file and reporting errors */
bool parse_file(const char * name, const char *file, const rule_t & rule){

    fit_t first(file);
    if (!first) {
      std::cerr << name << ": can't read " << file << '\n'; 
      return false;
    }
    fit_t last = first.make_end();
    if (first==last) {
      std::cerr << name << ": empty file " << file << '\n'; 
      return false;
    }

    info_t res = parse(pit_t(first, last, file), pit_t(), rule);

    if (!res.full){
      pit_t it=res.stop;
      file_position fpos=it.get_position();

      std::cerr << name << ": can't parse: " << fpos.file <<
        " at line: " << fpos.line <<
        " column: " << fpos.column << "\n";
      for (int i=0; i<40; i++) {
        std::cerr << (*it == '\n' ? ' ': *it);
        it++;
      }
      std::cerr << "\n";
      return false;
    }
    return true;
}
