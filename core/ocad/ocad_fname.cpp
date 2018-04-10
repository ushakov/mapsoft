#include "ocad_fname.h"
#include "ocad_types.h"
#include "err/err.h"
#include <iostream>

using namespace std;

namespace ocad{

void
ocad_fname::read(FILE *F, int addr, int size){
  if (addr==0) return;

  if (fseek(F, addr, SEEK_SET)!=0)
      throw Err() << "can't seek file to fname block";

  char * buf = new char [size];
  if (fread(buf, 1, size, F)!=size)
      throw Err() << "can't read fname block";
  string s(buf, buf+size);
  delete[] buf;

  data = iconv_win.to_utf8(s);
}

int
ocad_fname::write(FILE *F) const{
  int pos = ftell(F);
  string s = iconv_win.from_utf8(data);
  if (fwrite(s.data(), 1, s.size(), F)!=s.size())
    throw Err() << "error while writing fname";
  return pos;
}

void
ocad_fname::dump(int verb){
  if (verb<1) return;
  cout << "fname: "
    << data.c_str() << "\n";
}


} // namespace
