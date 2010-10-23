#ifndef OCAD_SYMBOL8_H
#define OCAD_SYMBOL8_H

#include <cstdio>
#include "ocad_types.h"
#include "ocad_symbol.h"

namespace ocad{

// someting wrong with this structure in documentation (the tail is shifted by 1byte)
struct _ocad8_base_symb{
  ocad_small size;   // size of the symbol in bytes (depends on the type and the number of subsymbols)
  ocad_small sym;    // symbol number (1010 for 101.0)
  ocad_small type;   // object type
                     // 1: point, 2: line or line text, 3: area, 4: text, 5: rectangle
  ocad_byte stype;   // symbol type
                     // 1: line text and text, 2: other
  ocad_byte flags;   // OCAD 6/7: must be 0
                     // OCAD 8: 1: not oriented to north 2: icon is compressed
  ocad_small extent; // how much the rendered symbols can reach outside the
                     // coordinates of an object with this symbol
  ocad_bool selected;// symbol is selected in the symbol box
  ocad_byte status;  // status of the symbol: 0: Normal, 1: Protected, 2: Hidden
  ocad_small tool;   // v7,v8 lines: Preferred drawing tool
                        //         0: off
                        //         1: Curve mode
                        //         2: Ellipse mode
                        //         3: Circle mode
                        //         4: Rectangular mode
                        //         5: Straight line mode
                        //         6: Freehand mode
  ocad_byte r2;
  ocad_long pos;     // file position, not used in the file, only when loaded in memory
  ocad_byte colors[32]; // used colors (256bit). color 0 - lowest bit of the first byte
  char desc[32];        // description of the symbol (c-string?!)
  ocad_byte icon[264];  // in v8 can by uncompressed (16-bit colors) or compressed (256 color palette)

  _ocad8_base_symb(){
    assert(sizeof(*this) ==348);
    memset(this, 0, sizeof(*this));
  }
} __attribute__((packed));


struct ocad8_symbol : ocad_symbol{

  // index block entry type
  struct index{
    ocad_long pos; // file position
    index(): pos(0){}
  };

  ocad8_symbol(){}
  ocad8_symbol(const ocad_symbol & o):ocad_symbol(o){}

  void read(FILE * F, index idx, int v){
    int pos = ftell(F);
    _ocad8_base_symb s;
    if (fread(&s, 1, sizeof(s), F)!=sizeof(s))
      throw "can't read object\n";
    int size = s.size;

    // convert to OCAD9  XXXY -> XXX00Y
    sym = (s.sym/10)*1000 + s.sym%10;

    type = s.type;
    // convert type:
    // 2: line or line text -> 2 or 6
    // 5: formatted text or rectangle -> 5 or 7
    if ((type==2)&&(s.stype==1)) type = 6;
    if ((type==5)&&(s.stype!=1)) type = 7;

    extent = s.extent;
    desc = iconv_win.to_utf8(str_pas2str(s.desc,32));

    if (fseek(F, pos, SEEK_SET)!=0)
      throw "can't seek file to read symbol blob";
    char *buf = new char [size];
    if (fread(buf, 1, size, F)!=size)
        throw "can't read symbol blob";
    blob = std::string(buf, buf+size);
    delete [] buf;
  }

  index write(FILE * F, int v) const{
    if (fwrite(blob.data(), 1, blob.size(), F)!=blob.size())
      throw "can't write symbol blob";

    return index();
  }
};

} // namespace
#endif
