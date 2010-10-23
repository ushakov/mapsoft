#ifndef OCAD_STRING_H
#define OCAD_STRING_H

#include <cstdio>
#include <iostream>
#include "ocad_types.h"
#include <boost/lexical_cast.hpp>

namespace ocad{

struct ocad_string{

  // index block entry type
  struct index{
    ocad_long pos;  // file position of string
    ocad_long len;  // length reserved for the string
    ocad_long type; // string typ number, if < 0 then deleted
    ocad_long obj;  // number of the object from 1 (or 0)
    index(): pos(0),len(0),type(0),obj(0){ }
  };

  ocad_long type;
  ocad_long obj;
  std::string data;

  ocad_string():type(0),obj(0){}


  std::string get_field(const char f = 0) const{
    if (f==0){
      return std::string(data.begin(), data.begin() + data.find('\t', 0));
    }
    int tab=-1;
    do {
      tab = data.find('\t', tab+1);
      if (tab<0) break;
      if ((tab+2<data.size()) && (data[tab+1]==f)){
        int first = tab+2;
        int next = data.find('\t', tab+1);
        return std::string(data.begin() + first,
           next>0 ? data.begin() + next : data.end());
      }
    } while (tab+1<data.size());
    return std::string();
  }

  template <typename T>
  T get(const char f = 0) const{
    return boost::lexical_cast<T>(get_field(f));
  }

  void read(FILE * F, index idx, int v){
    char * buf = new char [idx.len];
    if (fread(buf, 1, idx.len, F)!=idx.len)
      throw "can't read string\n";
    data=std::string(buf, (char *)memchr(buf, 0, idx.len));
    delete[] buf;

    data = iconv_win.to_utf8(data);
    type = idx.type;
    obj = idx.obj;
  }

  index write(FILE * F, int v) const{
    index idx;
    idx.type = type;
    idx.obj = obj;
    std::string str = iconv_win.from_utf8(data);
    idx.len = str.length() + 1;

    char * buf = new char [idx.len];
    memset(buf, 0, idx.len);
    for (int i=0; i<idx.len; i++) buf[i]=str[i];
    if (fwrite(buf, 1, idx.len, F)!=idx.len)
      throw "can't write string";
    delete[] buf;

    return idx;
  }

  void dump(int verb) const{
    if (verb<1) return;
    std::cout << "  " << type;
    if (obj) std::cout << " obj: " << obj;
    std::cout << " data: " << data << "\n";
  }
};

} // namespace
#endif
