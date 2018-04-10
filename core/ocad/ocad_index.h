#ifndef OCAD_INDEX_H
#define OCAD_INDEX_H

#include <cstdio>
#include <vector>
#include <cassert>
#include "ocad_types.h"
#include "err/err.h"


namespace ocad{

/// OCAD index block structure: address of the next block +
/// 256 index entries with object addresses
/// Index entry type is different for various objects.
template <typename B>
struct _ocad_index{ // 1028 bytes
  ocad_long next;     // file position of the next symbol block or 0
  B index[256];       // index data

  _ocad_index(): next(0){
    assert(sizeof(*this) == 4+256*sizeof(B));
  }
};

/// Class for OCAD index
template <typename T>
struct ocad_index: std::vector<T>{

  void read(FILE * F, int addr, int v){
    check_v(&v);
    while (addr!=0){
      if (fseek(F, addr, SEEK_SET)!=0)
        throw Err() << "can't seek file to index block";
      _ocad_index<typename T::index> bl;
      if (fread(&bl, 1, sizeof(bl), F)!=sizeof(bl))
        throw Err() << "can't read index block";
      addr=bl.next;
      for(int i=0; i<256; i++){
        int pos = bl.index[i].pos;
        if (pos==0) continue;
        if (fseek(F, pos, SEEK_SET)!=0)
          throw Err() << "can't seek file to read index entry";
        bl.index[i].pos=0;
        T s;
        s.read(F, bl.index[i], v);
        std::vector<T>::push_back(s);
      }
    }
  }

  int write(FILE * F, int v) const{
    check_v(&v);
    int first_pos = ftell(F);

    int n=0;
    while(1){
      _ocad_index<typename T::index> bl;

      int bl_pos=ftell(F);
      if (fseek(F, sizeof(bl), SEEK_CUR)!=0)
         throw Err() << "can't seek file to write index block";

      int i;
      for (i=0; i<256; i++){
        if (i+n>=this->size()) break;
        int p = ftell(F);
        bl.index[i] = (*this)[i+n].write(F, v);
        bl.index[i].pos = p;
      }
      if (i+n<this->size())
        bl.next=ftell(F);

      int last_pos = ftell(F);
      if (fseek(F, bl_pos, SEEK_SET)!=0)
         throw Err() << "can't seek file to write index block";
      if (fwrite(&bl, 1, sizeof(bl), F)!=sizeof(bl))
         throw Err() << "can't write index block";
      if (fseek(F, last_pos, SEEK_SET)!=0)
         throw Err() << "can't seek file to write index block";

      if (i+n>=this->size()) break;
      n+=256;
    }
    return first_pos;
  }

  void dump(int verb) const{
    if (verb<1) return;
    std::cout << this->size() << " index entries\n";
    typename std::vector<T>::const_iterator i;
    for (i = this->begin(); i != this->end(); i++) i->dump(verb);
  }

};


} // namespace
#endif
