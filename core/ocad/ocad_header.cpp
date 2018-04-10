#include <iostream>
#include <cassert>
#include <iostream>
#include "ocad_header.h"
#include "err/err.h"

using namespace std;

namespace ocad{

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

ocad_header::ocad_header(){
  memset(this, 0, sizeof(*this));
}

void
ocad_header::read(FILE *F){
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

ocad_header::ocad_header(FILE *F){
  memset(this, 0, sizeof(*this));
  this->read(F);
}

void
ocad_header::write(FILE *F) const{
  if ((version<6)||(version>9))
    throw Err() << "unsupported OCAD version";
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
      cerr << "OCAD9: skipping v6..8 objects!\n";
  }
  else {
    h.obj_pos = obj8_pos;
    if (obj9_pos)
      cerr << "OCAD6..8: skipping v9 objects!\n";
  }

  if (version > 7){
    h.str_pos = str_pos;
  }
  else{
    if (str_pos)
      cerr << "OCAD6..7: skipping strings!\n";
  }

  if (version>8){
    h.fname_pos=fname_pos;
    h.fname_size=fname_size;

    if (setup_pos || setup_size)
      cerr << "OCAD9: skipping setup block!\n";
    if (info_pos || info_size)
      cerr << "OCAD9: skipping info block!\n";
  }
  else {
    h.setup_pos = setup_pos;
    h.setup_size = setup_size;

    h.info_pos = info_pos;
    h.info_size = info_size;

    if (fname_pos || fname_size)
      cerr << "OCAD6..8: skipping fname block!\n";
  }

  if (fwrite(&h, 1, sizeof(h), F)!=sizeof(h))
    throw Err() << "error while writing header";
}

void
ocad_header::seek(FILE *F) const{
  if (fseek(F, sizeof(_ocad_header), SEEK_SET)!=0)
    throw Err() << "can't seek file to skip header";
}

void
ocad_header::dump(int verb){
  if (verb<1) return;
  cout
    << "OCAD " << version << "." << subversion << ", "
    << (type?"course setting":"normal map") << "\n";
  if (verb<2) return;
  cout
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

} // namespace
