#ifndef OCAD_OBJECT8_H
#define OCAD_OBJECT8_H

#include <cstdio>
#include <cassert>
#include "ocad_object.h"

namespace ocad{

// OCAD8 object = OCAD object + low-level index struct + read/write functions
struct ocad8_object : ocad_object{

  /// Index entry type and read/write functions are used in ocad_index
  /// template for reading/writing OCAD files.

  /// Low-level index entry type.
  struct index{
    ocad_coord lower_left, upper_right;
    ocad_long pos;  // file position
    ocad_word len;  // OCAD 6 and 7: size of the object in the file in bytes
                    // OCAD 8: number of coordinate pairs. size = 32+8*len
                    // this is reserved length, real length may be shorter
    ocad_small sym; // the symbol number (0 for deleted objects)
    index(){
      assert(sizeof(*this) == 24);
      memset(this, 0, sizeof(*this));
    }
  };

  /// Empty constructor.
  ocad8_object();

  /// Construct from ocad_object (trivial).
  ocad8_object(const ocad_object & o);


  /// Check data and text limits
  void check_limits(ocad_small &n, ocad_small &t, int lt, int ls) const;

  void read(FILE * F, index idx, int v);

  index write(FILE * F, int v) const;
};

} // namespace
#endif
