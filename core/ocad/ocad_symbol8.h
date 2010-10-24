#ifndef OCAD_SYMBOL8_H
#define OCAD_SYMBOL8_H

#include <cstdio>
#include "ocad_types.h"
#include "ocad_symbol.h"

namespace ocad{

struct ocad8_symbol : ocad_symbol{

  /// Index entry type and read/write functions are used in ocad_index
  /// template for reading/writing OCAD files.

  /// Low-level index entry type.
  struct index{
    ocad_long pos; // file position
    index(): pos(0){}
  };

  ocad8_symbol();
  ocad8_symbol(const ocad_symbol & o);

  void read(FILE * F, index idx, int v);

  index write(FILE * F, int v) const;
};

} // namespace
#endif
