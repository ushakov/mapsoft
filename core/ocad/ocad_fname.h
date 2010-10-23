#ifndef OCAD_FNAME_H
#define OCAD_FNAME_H

#include <cstdio>
#include "ocad_types.h"

namespace ocad{

/*
OCAD9 file name data block
*/

/// OCAD 

struct ocad_fname {

  std::string data;

  void read(FILE *F, int addr, int size){
    if (addr==0) return;

    if (fseek(F, addr, SEEK_SET)!=0)
        throw "can't seek file to fname block";

    char * buf = new char [size];
    if (fread(buf, 1, size, F)!=size)
        throw "can't read fname block\n";
    std::string s(buf, buf+size);
    delete[] buf;

    data = iconv_win.to_utf8(s);
  }

  int write(FILE *F) const{
    int pos = ftell(F);
    std::string s = iconv_win.from_utf8(data);
    if (fwrite(s.data(), 1, s.size(), F)!=s.size())
      throw "error while writing fname";
    return pos;
  }

  void dump(int verb){
    if (verb<1) return;
    std::cout << "fname: "
      << data.c_str() << "\n";
  }
};

} // namespace
#endif
