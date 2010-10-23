#ifndef OCAD_OBJECT8_H
#define OCAD_OBJECT8_H

#include <cstdio>
#include "ocad_types.h"
#include "ocad_object.h"

namespace ocad{

/// OCAD8 object low-level structure
struct _ocad8_object{
  ocad_small sym;    // symbol number
  ocad_byte type;    // object type (1-pt, ...)
  ocad_byte unicode; // OCAD 6/7: must be 0, OCAD 8: 1 if the text is Unicode
  ocad_small n;      // number of coordinates
  ocad_small nt;     // number of coordinates used for text
  ocad_small ang;    // Angle, unit is 0.1 degrees (for points, area with structure,.
                     // text, rectangles)
  ocad_small r2;     // reserved
  ocad_long rh;      // reserved for height
  char res[16];
  _ocad8_object(){
    assert(sizeof(*this) == 32);
    memset(this, 0, sizeof(*this));
  }
};

// OCAD8 object = OCAD object + low-level index struct + read/write functions
struct ocad8_object : ocad_object{

  // index block entry type
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

  ocad8_object(){}
  ocad8_object(const ocad_object & o):ocad_object(o){}

/*
  limits:
  OCAD 6,7
  n+nt <= 2000
  nt <= 1024 (8191 characters + terminating zero)
  Index.Len := 32 + 8 * n + 8 * nt;

  OCAD 8
  n + nt <= 32768
  nt <= 1024 (8191 characters + terminating zero)
  Index.Len := n+nt
*/
  void check_limits(ocad_small &n, ocad_small &t, int lt, int ls) const{
    lt--; // we need space for terminatng zero
    if (t > lt){ std::cerr
      << "warning: cutting too long text: " << t << " -> " << lt << "\n";
      t=lt;
    }
    if (n+t > ls){ std::cerr
      << "warning: cutting too long coords: " << n << " -> " << ls-t << "\n";
      n = ls-t;
    }
  }

  void read(FILE * F, index idx, int v){

    *this = ocad8_object(); // reset data
    _ocad8_object o;
    if (fread(&o, 1, sizeof(o), F)!=sizeof(o))
      throw "can't read object\n";

    if (idx.sym==0){ // deleted object
      status=0;
    }
    else if ((idx.sym & 0x3FFF) !=o.sym){ // TODO - what is the mask?!
       std::cerr << "warning: symbols in object and in index differ: "
        << o.sym << " vs " << idx.sym << "\n"; 
    }

    // convert to OCAD9  XXXY -> XXX00Y
    sym = (o.sym/10)*1000 + o.sym%10;


    type = o.type;
    // convert type:
    // 2: line or line text -> 2 or 6
    // 5: formatted text or rectangle -> 5 or 7
    if ((type==2)&&(o.nt!=0)) type = 6;
    if ((type==5)&&(o.nt==0)) type = 7;

    ang = o.ang;

    // limits
    if (v==8){
      if (idx.len != o.n + o.nt) std::cerr
        << "warning: object length does not match coord number!\n";
      check_limits(o.n, o.nt, 1024, 32768);
    }
    else{
      if (idx.len != sizeof(o) + (o.n + o.nt) * sizeof(ocad_coord)) std::cerr
        << "warning: object length does not match coord number!\n";
      check_limits(o.n, o.nt, 1024, 2000);
    }

    read_coords(F, o.n);
    read_text(F, o.nt);

    if (o.unicode) text=iconv_uni.to_utf8(text);
    else           text=iconv_win.to_utf8(text);

    dump(2);
  }


  index write(FILE * F, int v) const{
    _ocad8_object o;

    // convert sym from OCAD9  XXXZZY -> XXXY. this may couse collisions!
    // TODO - check symbol collisions before writing OCAD6..8 objects
    o.sym = (sym/1000)*10 + sym%10;

    // convert type:
    // line text 6 -> 2
    // rectangle 7 -> 5
    o.type=type;
    if (o.type == 6) o.type = 2;
    if (o.type == 7) o.type = 5;

    o.ang=ang;

    std::string txt;
    if (v>7){
      o.unicode=1;
      txt = iconv_uni.from_utf8(text);
    }
    else{
      txt = iconv_win.from_utf8(text);
    }

    o.n  = coords.size();
    o.nt = txt_blocks(txt);

    // limits
    if (v==8) check_limits(o.n, o.nt, 1024, 32768);
    else      check_limits(o.n, o.nt, 1024, 2000);

    if (fwrite(&o, 1, sizeof(o), F)!=sizeof(o))
      throw "can't write object";

    write_coords(F, o.n);
    write_text(txt, F, o.nt);

    // create and return index entry
    index ret;
    ret.upper_right=URC(); // todo - pump by symbol extent
    ret.lower_left=LLC();
    if (v < 8) ret.len = sizeof(o) + (o.n + o.nt) * sizeof(ocad_coord);
    else ret.len = o.n + o.nt;
    ret.sym = o.sym;
    if (status==0) ret.sym=0; // deleted object
    return ret;
  }

};

} // namespace
#endif
