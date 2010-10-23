#ifndef OCAD_HEADER_H
#define OCAD_HEADER_H

#include <cstdio>
#include <cassert>
#include "ocad_types.h"

namespace ocad{

/*
OCAD header has a compatable structure for versions 6..9.
It contains version information and addresses of
data in the file.
There are defferent data blocks in different ocad versions.
Unused data addresses are set to 0.
*/

/// OCAD 6..9 header low-level structure
struct _ocad_header{ // 48 bytes
  ocad_small ocad_mark;   // 3245 (hex 0cad)
  ocad_byte  ftype;       // v6: 0
                          // v7: 7
                          // v8: 2: normal map, 3: course setting
                          // v9: 0: normal map, 1: course setting
  ocad_byte  fstatus;     // not used
  ocad_small version;     // 6..9
  ocad_small subversion;  // (0 for 9.00, 1 for 9.1 etc.)
  ocad_long  sym_pos;     // file position of the first symbol index block
  ocad_long  obj_pos;     // file position of the first index block
  ocad_long  setup_pos;   // OCAD 6/7/8: file position of the setup record
                          // OCAD 9: reserved
  ocad_long  setup_size;  // OCAD 6/7/8: size (in bytes) of the setup record
                          // OCAD 9: reserved
  ocad_long  info_pos;    // OCAD 6/7/8: file position of the file information (0-term string up to 32767
                          // OCAD 9: reserved
  ocad_long  info_size;   // OCAD 6/7/8: size (in bytes) of the file information
                          // OCAD 9: reserved
  ocad_long  str_pos;     // OCAD 8/9: file position of the first string index block
  ocad_long  fname_pos;   // OCAD 9: file position of file name, used for temporary files
                          //         and to recovery the file
  ocad_long  fname_size;  // OCAD 9: size of the file name, used for temporary files only
  ocad_long  r4;          // reserved
  _ocad_header(){
    assert(sizeof(*this) == 48);
    memset(this, 0, sizeof(*this));
    ocad_mark=0x0CAD;
  }
};

struct ocad_header {

  /// file version
  ocad_int version, subversion;

  /// file type
  ocad_int type; // 0 for normal map, 1 for course setting

  /// posinitons of data structures
  ocad_int sym_pos, obj8_pos, obj9_pos, str_pos;

  /// OCAD 6..8 only
  ocad_int setup_pos, setup_size, info_pos, info_size;

  /// OCAD 9 only
  ocad_int fname_pos, fname_size;


  /// Empty constructor.
  ocad_header(){
    memset(this, 0, sizeof(*this));
  }

  /// Rewind file and read header.
  /// set OCAD version 0 on errors.
  /// set data positions.
  void read(FILE *F){
    *this = ocad_header(); // reset all values
    rewind(F);
    _ocad_header h;

    if ((fread(&h, 1, sizeof(h), F)!=sizeof(h)) ||
      (h.ocad_mark!=0x0CAD)) return;

    version = h.version;
    subversion = h.subversion;

    // unsupported versions - return with zero addresses
    if ((version <6) || (version >9)) return;

    if ( ((version == 8) && (h.ftype==3)) ||
         ((version == 9) && (h.ftype==1)) ) type=1;
    else type = 0;

    //  set positions
    sym_pos=h.sym_pos;

    if (version>8){
       obj9_pos=h.obj_pos;
    }
    else {
      obj8_pos=h.obj_pos;
    }

    if (version>7){
      str_pos=h.str_pos;
    }

    if (version>8){
      fname_pos = h.fname_pos;
      fname_size = h.fname_size;
    }
    else {
      // shead_pos = sizeof(h);

      setup_pos = h.setup_pos;
      setup_size = h.setup_size;

      info_pos = h.info_pos;
      info_size = h.info_size;
    }
  }

  ocad_header(FILE *F){
    memset(this, 0, sizeof(*this));
    this->read(F);
  }


  /// Rewind file and write header.
  void write(FILE *F) const{
    if ((version<6)||(version>9))
      throw "unsupported OCAD version";
    rewind(F);
    _ocad_header h;
    h.version = version;
    h.subversion = subversion;
    if (version==6) h.ftype=0;
    if (version==7) h.ftype=7;
    if (version==8) h.ftype=2;
    if (version==9) h.ftype=0;

    h.sym_pos = sym_pos;

    if (version>8){
      h.obj_pos = obj9_pos;
      if (obj8_pos)
        std::cerr << "OCAD9: skipping v6..8 objects!\n";
    }
    else {
      h.obj_pos = obj8_pos;
      if (obj9_pos)
        std::cerr << "OCAD6..8: skipping v9 objects!\n";
    }

    if (version > 7){
      h.str_pos = str_pos;
    }
    else{
      if (str_pos)
        std::cerr << "OCAD6..7: skipping strings!\n";
    }

    if (version>8){
      h.fname_pos=fname_pos;
      h.fname_size=fname_size;

      if (setup_pos || setup_size)
        std::cerr << "OCAD9: skipping setup block!\n";
      if (info_pos || info_size)
        std::cerr << "OCAD9: skipping info block!\n";
    }
    else {
      h.setup_pos = setup_pos;
      h.setup_size = setup_size;

      h.info_pos = info_pos;
      h.info_size = info_size;

      if (fname_pos || fname_size)
        std::cerr << "OCAD6..8: skipping fname block!\n";
    }

    if (fwrite(&h, 1, sizeof(h), F)!=sizeof(h))
      throw "error while writing header";
  }

  void seek(FILE *F) const{
    if (fseek(F, sizeof(_ocad_header), SEEK_SET)!=0)
      throw "can't seek file to skip header";
  }

  void dump(int verb){
    if (verb<1) return;
    std::cout
      << "OCAD " << version << "." << subversion << ", "
      << (type?"course setting":"normal map") << "\n";
    if (verb<2) return;
    std::cout
      << "data blocks addresses:\n"
      << "  symbols:    " << sym_pos << "\n"
      << "  objects v8: " << obj8_pos << "\n"
      << "  objects v9: " << obj9_pos << "\n"
      << "  strings:    " << str_pos << "\n"
      << "  setup v8:   " << setup_pos << ":" << setup_size << "\n"
      << "  info  v8:   " << info_pos << ":" << info_size << "\n"
      << "  fname v9:   " << fname_pos << ":" << fname_size << "\n"
    ;
  }
};

} // namespace
#endif
