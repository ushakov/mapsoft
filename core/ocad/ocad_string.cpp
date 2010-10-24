#include <cstdio>
#include <iostream>
#include "ocad_string.h"

using namespace std;

namespace ocad{

ocad_string::ocad_string():type(0),obj(0){}

string
ocad_string::get_str(const char f) const{
  if (f==0){
    return string(data.begin(), data.begin() + data.find('\t', 0));
  }
  int tab=-1;
  do {
    tab = data.find('\t', tab+1);
    if (tab<0) break;
    if ((tab+2<data.size()) && (data[tab+1]==f)){
      int first = tab+2;
      int next = data.find('\t', tab+1);
      return string(data.begin() + first,
         next>0 ? data.begin() + next : data.end());
    }
  } while (tab+1<data.size());
  return string();
}

void
ocad_string::dump(int verb) const{
  if (verb<1) return;
  cout << "  " << type;
  if (obj) cout << " obj: " << obj;
  cout << " data: " << data << "\n";
}

void
ocad_string::read(FILE * F, ocad_string::index idx, int v){
  char * buf = new char [idx.len];
  if (fread(buf, 1, idx.len, F)!=idx.len)
    throw "can't read string\n";
  data=string(buf, (char *)memchr(buf, 0, idx.len));
  delete[] buf;

  data = iconv_win.to_utf8(data);
  type = idx.type;
  obj = idx.obj;
}

ocad_string::index
ocad_string::write(FILE * F, int v) const{
  ocad_string::index idx;
  idx.type = type;
  idx.obj = obj;
  string str = iconv_win.from_utf8(data);
  idx.len = str.length() + 1;

  char * buf = new char [idx.len];
  memset(buf, 0, idx.len);
  for (int i=0; i<idx.len; i++) buf[i]=str[i];
  if (fwrite(buf, 1, idx.len, F)!=idx.len)
    throw "can't write string";
  delete[] buf;

  return idx;
}

} // namespace
