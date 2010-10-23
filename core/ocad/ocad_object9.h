#ifndef OCAD_OBJECT9_H
#define OCAD_OBJECT9_H

#include <cstdio>
#include <cassert>
#include "ocad_types.h"
#include "ocad_object.h"

namespace ocad{

// OCAD9 object = OCAD object + low-level index struct + read/write functions
struct ocad9_object : ocad_object{

  // index block entry type (for using with ocad_index)
  struct index {
    ocad_coord lower_left;
    ocad_coord upper_right;
    ocad_long pos;   // file position
    ocad_long len;   // number of coordinate pairs. size = 32+8*len
                     // this is reserved length, real length may be shorter
    ocad_long sym;   // the symbol number (0 for deleted objects)
                     // -3 = image object eg AI object
                     // -2 = graphic object
                     // -1 = imported, no symbol assigned or symbol number
    ocad_byte type;  // 1: point
                     // 2: line
                     // 3: area
                     // 4: unformatted text
                     // 5: formatted text
                     // 6: line text
                     // 7: rectangle
    ocad_byte r1;    // reserved
    ocad_byte status;// 0: deleted
                     // 1: normal
                     // 2: hidden
                     // 3: deleted for undo
    ocad_byte viewtype;
                     // 0: normal
                     // 1: course setting object
                     // 2: modified preview object
                     // 3: unmodified preview object
                     // 4: temporary object (symbol editor or control description)
                     // 10: DXF import, GPS import
    ocad_small col;  // symbolized objects: color number
                     // graphic object: color number
                     // image object: CYMK color of object
    ocad_small r2;   // reserved
    ocad_small implayer; //  Layer number of imported objects
    ocad_small r3;

    index(){
      assert(sizeof(*this) == 40);
      memset(this, 0, sizeof(*this));
    }
  };

  ocad9_object();
  ocad9_object(const ocad_object & o);

  void read(FILE * F, index idx, int v);

  index write(FILE * F, int v) const;
};

} // namespace
#endif
