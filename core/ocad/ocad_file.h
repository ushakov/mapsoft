#ifndef OCAD_FILE_H
#define OCAD_FILE_H

#include <cstdio>
#include "ocad_header.h"
#include "ocad_shead.h"
#include "ocad_symbol.h"
#include "ocad_symbol8.h"
#include "ocad_symbol9.h"
#include "ocad_string.h"
#include "ocad_object8.h"
#include "ocad_object9.h"
#include "ocad_index.h"
#include "ocad_fname.h"

#include "2d/rect.h"

namespace ocad{

typedef ocad_index<ocad_string>  ocad_strings;
typedef ocad_index<ocad_object>  ocad_objects;
typedef ocad_index<ocad_symbol>  ocad_symbols;

/// It is a structure with all passible data blocks.
/// It can be read or write from files with
/// different versions. It is not possible to
/// read and write different versions yet
/// (the main problem is different symbol format)

struct ocad_file{

  // all possible data blocks
  int v, sv, t;
  ocad8_shead     shead;      // v 6 7 8
  ocad_symbols    symbols;    // incompat!
  ocad_objects    objects;    // good!
  ocad_strings    strings;    // so-so, v 8,9
  ocad_fname      fname;      // v 9
  // ocad_setup               // v 6 7 8
  // ocad_info                // v 6 7 8

  // read ocad 6,7,8,9, cast to ocad 9 structures -- todo
  // verb = 0 - silent
  // verb = 1 - some info
  // verb = 2 - dump all known data from file
  void read(const char * fn, int verb=0);

  void write (const char * fn) const;

  // do it before writing or calculating range...
  void update_extents();

  iRect range() const;
};


} // namespace
#endif