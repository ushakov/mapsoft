#include <iostream>
#include "spirit_utils.h"

/** function for parsing file and reporting errors */
bool parse_file(const char * name, const char *file, const rule_t & rule){

    iterator_t first(file);
    if (!first) { 
	std::cerr << name << ": can't find file " << file << '\n'; 
	return false;
    }
    iterator_t last = first.make_end();

    info_t res = parse(first, last, rule);

    if (!res.full){
      iterator_t it = it;
      std::cerr << name << ": can't parse file " << file <<
        " at position " << res.length << "\n";
      int i,j;
      for (i=0; (i>-20) && (it!=first); i--) if (it-- == first) break;
      for (j=i; j<20; j++) {
        std::cerr << (*it == '\n' ? '\f':*it);
        if (it++ == last) break;
      }
      std::cerr << "\n";
      for (j=i; j<20; j++) std::cerr << (j==0? "^":" ");
      std::cerr << "\n";
      return false;
    }
    return true;
}
