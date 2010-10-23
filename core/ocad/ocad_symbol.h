#ifndef OCAD_SYMBOL_H
#define OCAD_SYMBOL_H

#include <cstdio>
#include "ocad_types.h"

namespace ocad{

struct ocad_symbol{

  ocad_long sym;    // symbol number
  ocad_byte type;
  ocad_long extent;

  std::string blob;
  std::string desc;

  void dump(int verb) const{
    if (verb<1) return;
    std::cout 
      << " sym: " << sym/1000 << "." << sym%1000
      << " type: " << (int)type
      << " extent: " << extent
      << " size: " << blob.size()
      << " \"" << desc
      << "\"\n";
    // ...
  }
};

} // namespace
#endif
