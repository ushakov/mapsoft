#include <cassert>
#include <iostream>
#include "ocad_symbol9.h"
#include "err/err.h"

using namespace std;

namespace ocad{

// Someting wrong with this structure in documentation
// (the tail is shifted by 1byte).
struct _ocad9_base_symb{
  ocad_long size;   // size of the symbol in bytes (depends on the type and the number of subsymbols)
                    // Coordinates following the symbol are included.
  ocad_long sym;    // symbol number (101000 for 101.0, 101005 for 101.5, 101123 for 101.123)
  ocad_byte type;   // object type
                     // 1: point, 2: line, 3: area, 4: text
                     // 6: line text, 7: rectangle
  ocad_byte flags;   // 1: rotatable symbol (not oriented to north)
                     // 4: belongs to favorites
  ocad_bool selected;// symbol is selected in the symbol box
  ocad_byte status;  // status of the symbol: 0: Normal, 1: Protected, 2: Hidden
  ocad_byte tool;    // Preferred drawing tool
                        //         0: off
                        //         1: Curve mode
                        //         2: Ellipse mode
                        //         3: Circle mode
                        //         4: Rectangular mode
                        //         5: Straight line mode
                        //         6: Freehand mode
  ocad_byte csmode;  // Course setting mode:
                        //   0: Not used for course setting
                        //   1: course symbol
                        //   2: control description symbol
  ocad_byte sctype;  // Course setting object type
                       //   0: Start symbol (Point symbol)
                       //   1: Control symbol (Point symbol)
                       //   2: Finish symbol (Point symbol)
                       //   3: Marked route (Line symbol)
                       //   4: Control description symbol (Point symbol)
                       //   5: Course Titel (Text symbol)
                       //   6: Start Number (Text symbol)
                       //   7: Variant (Text symbol)
                       //   8: Text block (Text symbol)
//  ocad_byte csflags; // Course setting control description flags
                       //   a combination of the flags
                       //   32: available in column C
                       //   16: available in column D
                       //   8: available in column E
                       //   4: available in column F
                       //   2: available in column G
                       //   1: available in column H
  ocad_long extent;  // Extent how much the rendered symbols can reach outside the
                     // coordinates of an object with this symbol. For a point
                     // object it tells how far away from the coordinates of the
                     // object anything of the point symbol can appear.
  ocad_long pos;     // used internally
  ocad_small group;  // Group ID in the symbol tree. Lower and higher 8 bit are
                     // used for 2 different symbol trees.
  ocad_small ncolors; // Number of colors of the symbol max. 14 ///???
                      //   -1: the number of colors is > 14
  ocad_small colors[14]; // number of colors of the symbol
  char desc[32];        // description of the symbol (c-string?!)
  ocad_byte icon[484];  //  256 color palette (icon 22x22 pixels)

  _ocad9_base_symb(){
    assert(sizeof(*this) ==572);
    memset(this, 0, sizeof(*this));
  }
} __attribute__((packed));

ocad9_symbol::ocad9_symbol(){}
ocad9_symbol::ocad9_symbol(const ocad_symbol & o):ocad_symbol(o){}

void
ocad9_symbol::read(FILE * F, ocad9_symbol::index idx, int v){
  int pos = ftell(F);
  _ocad9_base_symb s;
  if (fread(&s, 1, sizeof(s), F)!=sizeof(s))
    throw Err() << "can't read object";
  int size = s.size;
  sym = s.sym;
  type = s.type;
  extent = s.extent;
  desc = iconv_win.to_utf8(str_pas2str(s.desc, 32));

  blob_version = v;
  if (fseek(F, pos, SEEK_SET)!=0)
    throw Err() << "can't seek file to read symbol blob";
  char *buf = new char [size];
  if (fread(buf, 1, size, F)!=size)
      throw Err() << "can't read symbol blob";
  blob = string(buf, buf+size);
  delete [] buf;
}

ocad9_symbol::index
ocad9_symbol::write(FILE * F, int v) const{
  if (blob_version > 8){
    if (fwrite(blob.data(), 1, blob.size(), F)!=blob.size())
      throw Err() << "can't write symbol blob";
  }
  else {
     cerr << "warning: skipping symbol with incompatible version\n";
  }
  return ocad9_symbol::index();
}

} // namespace
