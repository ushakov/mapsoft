#ifndef OCAD_OBJECT9_H
#define OCAD_OBJECT9_H

#include <cstdio>
#include "ocad_types.h"
#include "ocad_object.h"

namespace ocad{

/// OCAD9 object low-level structure
struct _ocad9_object{
  ocad_long  sym; // symbol number or typ
                  // image object= -3 (imported from ai or pdf, no symbol assigned)
                  //  graphic object = -2 (OCAD objects converted to graphics)
                  // imported object = -1 (imported, no symbol assigned)
  ocad_byte type; // object type (1-pt, ...)
  ocad_byte r1;   // reserved
  ocad_small ang; // Angle, unit is 0.1 degrees (for points, area with structure, 
                  // text, rectangles)
  ocad_long n;    // number of coordinates
  ocad_small nt;  // number of coordinates in the Poly array used for
                  // storing text in Unicode for (line text and text
                  // objects) if nText is > 0
                  // for all other objects it is 0
  ocad_small r2;  //
  ocad_long col;  // color number
  ocad_small width; // line width -- not used???
  ocad_small flags; // flages: LineStyle by lines -- not used???
  double r3, r4;
  _ocad9_object(){
    assert(sizeof(*this) == 40);
    memset(this, 0, sizeof(*this));
  }
};


// OCAD9 object = OCAD object + low-level index struct + read/write functions
struct ocad9_object : ocad_object{

  // index block entry type
  struct index{
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

  ocad9_object(){}
  ocad9_object(const ocad_object & o):ocad_object(o){}

  void read(FILE * F, index idx, int v){

    *this = ocad9_object(); // reset data
    _ocad9_object o;
    if (fread(&o, 1, sizeof(o), F)!=sizeof(o))
      throw "can't read object\n";

    if (idx.sym!=o.sym) std::cerr
      << "warning: symbols in object and in index differ: "
      << o.sym << " vs " << idx.sym << "\n";
    sym=o.sym;
    if (idx.type!=o.type) std::cerr
      << "warning: types in object and in index differ: "
      << o.type << " vs " << idx.type << "\n";
    type=o.type;

    status = idx.status;
    viewtype = idx.viewtype;
    idx_col = idx.col;
    implayer = idx.implayer;

    ang = o.ang;
    col = o.col;
    width = o.width;
    flags = o.flags;

    // is these fields used?
    if (width != 0)  std::cerr << "note: width!=0\n";
    if (flags != 0)  std::cerr << "note: flags!=0\n";

    if (idx.len != sizeof(o) + (o.n + o.nt) * sizeof(ocad_coord)) std::cerr
      << "warning: object length does not match coord number!\n";

    read_coords(F, o.n);
    read_text(F, o.nt);

    text=iconv_uni.to_utf8(text);

/*
std::cerr << "extent: "
  << idx.lower_left.getx() - LLC().getx() << "  "
  << idx.upper_right.getx() - URC().getx() << "  "
  << idx.lower_left.gety() - LLC().gety() << "  "
  << idx.upper_right.gety() - URC().gety() << "\n";
*/
    dump(2);
  }


  index write(FILE * F, int v) const{

    _ocad9_object o;
    o.sym=sym;
    o.type=type;
    o.ang=ang;
    o.col = col;
    o.width = width;
    o.flags = flags;

    std::string txt = iconv_uni.from_utf8(text);

    o.n  = coords.size();
    o.nt = txt_blocks(txt);

    if (fwrite(&o, 1, sizeof(o), F)!=sizeof(o))
      throw "can't write object";

    write_coords(F);
    write_text(txt, F);

    // create and return index entry
    index ret;
    ret.upper_right=URC(); // todo - pump by symbol extent
    ret.lower_left=LLC();
    ret.len = (o.n+o.nt) * sizeof(ocad_coord) + sizeof(o);
    ret.sym = sym;
    ret.type = type;
    ret.status = status;
    ret.viewtype = viewtype;
    ret.col = idx_col;
    ret.implayer = implayer;
    return ret;
  }

};

} // namespace
#endif
