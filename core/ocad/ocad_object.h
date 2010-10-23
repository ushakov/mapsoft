#ifndef OCAD_OBJECT_H
#define OCAD_OBJECT_H

#include <cstdio>
#include <vector>
#include <string>
#include "ocad_types.h"

namespace ocad{

// common structure for OCAD object
struct ocad_object{

  ocad_long  sym;
  ocad_byte  type;
  ocad_byte  status;
  ocad_byte  viewtype;
  ocad_small idx_col;
  ocad_small implayer;
  ocad_small ang;
  ocad_small col;
  ocad_small width;
  ocad_small flags;

  ocad_small extent; // TODO - not here?

  std::vector<ocad_coord> coords;
  std::string text;

  ocad_object(): sym(0), type(3), status(1), viewtype(0), implayer(0),
    idx_col(-1), ang(0), col(0), width(0), flags(0), extent(0){}

  void dump(int verb) const{
    if (verb<1) return;
    std::cout << "object: " << sym/1000 << "." << sym%1000
      << " type: " << (int)type
      << " col: " << idx_col << " - " << col;
    if (status!=1) std::cout << " status: " << (int)status;
    if (viewtype)  std::cout << " viewtype: " << (int)viewtype;
    if (implayer)  std::cout << " implayer: " << implayer;
    if (ang)       std::cout << " ang: " << ang/10.0;
    if (coords.size()) std::cout << " points: " << coords.size();
    if (text.length()) std::cout << " text: \""  << text << "\"";
    std::cout << "\n";
    if (verb<2) return;
    if (coords.size()){
      std::cout << "  coords: ";
      for (int i=0; i<coords.size(); i++) coords[i].dump(std::cout);
      std::cout << "\n";
    }
  }

  ocad_coord LLC() const{
    if (coords.size() == 0) return ocad_coord();
    ocad_coord ret;
    ret.setx(coords[0].getx());
    ret.sety(coords[0].gety());
    for (int i=0; i<coords.size(); i++){
      if (coords[i].getx() < ret.getx()) ret.setx(coords[i].getx());
      if (coords[i].gety() < ret.gety()) ret.sety(coords[i].gety());
    }
    ret.setx(ret.getx()-extent);
    ret.sety(ret.gety()-extent);
    return ret;
  }
  ocad_coord URC() const{
    if (coords.size() == 0) return ocad_coord();
    ocad_coord ret;
    ret.setx(coords[0].getx());
    ret.sety(coords[0].gety());
    for (int i=0; i<coords.size(); i++){
      if (coords[i].getx() > ret.getx()) ret.setx(coords[i].getx());
      if (coords[i].gety() > ret.gety()) ret.sety(coords[i].gety());
    }
    ret.setx(ret.getx()+extent);
    ret.sety(ret.gety()+extent);
    return ret;
  }

  // get number of 8-byte blocks needed for 0x0000-terminates string txt
  int txt_blocks(const std::string & txt) const{
    if (txt.length() == 0 ) return 0;
    return (txt.length()+2)/8+1;
  }

  void write_text(const std::string & txt, FILE *F, int limit=-1) const {
    int n=txt_blocks(txt);
    if (limit>=0) n=MIN(limit, n); // TODO: this can break unicode letters!
    if (n){

      char *buf = new char [n*8];
      memset(buf, 0, n*8);
      for (int i=0; i<txt.length(); i++) buf[i] = txt[i];
      if (fwrite(buf, 1, n*8, F)!=n*8)
        throw "can't write object text";
      delete [] buf;
    }
  }

  void write_coords(FILE *F, int limit=-1) const{
    int n = coords.size();
    if (limit>=0) n=MIN(limit, n);
    if (n){
      ocad_coord * buf = new ocad_coord[n];
      memset(buf, 0, n*8);
      for (int i=0; i<n; i++)  buf[i] = coords[i];
      if (fwrite(buf, sizeof(ocad_coord), n, F)!=n)
        throw "can't write object coordinates";
      delete [] buf;
    }
  }

  void read_coords(FILE *F, int n){
    if (n){
      ocad_coord * buf = new ocad_coord[n];
      if (fread(buf, sizeof(ocad_coord), n, F)!=n)
        throw "can't read object coordinates\n";
      coords = std::vector<ocad_coord>(buf, buf+n);
      delete [] buf;
    }
  }

  void read_text(FILE *F, int n){
    if (n){
      char *buf = new char [n*8];
      if (fread(buf, 1, n*8, F)!=n*8)
        throw "can't read object text\n";
      for (int i=0; i<n*8-1; i++){
        if ((buf[i]==0) && (buf[i+1]==0)) break; // 0x0000-terminated string
        text.push_back(buf[i]);
      }
      delete [] buf;
    }
  }

};

} // namespace
#endif
