#include "ocad_file.h"

#include <iostream>

using namespace std;

namespace ocad{

typedef ocad_index<ocad9_object> ocad9_objects;
typedef ocad_index<ocad8_object> ocad8_objects;
typedef ocad_index<ocad9_symbol> ocad9_symbols;
typedef ocad_index<ocad8_symbol> ocad8_symbols;


void
ocad_file::read(const char * fn, int verb){

  // open file
  FILE * F = fopen(fn, "r");
  if (!F) throw "can't open file";

  ocad_header h(F);
  h.dump(verb);

  v  = h.version;
  sv = h.subversion;
  t  = h.type;
  if (v==0) throw "not an OCAD file";
  if ((v<6)||(v>9)) throw "unsupported OCAD version";

  if (v<9){
    shead.read(F);
    shead.dump(verb);
    vector<ocad_string> s = shead.to_strings();
    strings.insert(strings.end(), s.begin(), s.end());
  }

  symbols.clear();
  if (v>8){
    ocad9_symbols s9;
    s9.read(F, h.sym_pos, v);
    symbols.insert(symbols.end(), s9.begin(), s9.end());
  }
  else{
    ocad8_symbols s8;
    s8.read(F, h.sym_pos, v);
    symbols.insert(symbols.end(), s8.begin(), s8.end());
  }
  symbols.dump(verb);

  ocad8_objects o8;
  ocad9_objects o9;
  o8.read(F, h.obj8_pos, v);
  o9.read(F, h.obj9_pos, v);
  objects.insert(objects.end(), o8.begin(), o8.end());
  objects.insert(objects.end(), o9.begin(), o9.end());

  objects.dump(verb);

  strings.read(F, h.str_pos, v);

  strings.dump(verb);

  if (v>8) fname.read(F, h.fname_pos, h.fname_size);

  fname.dump(verb);

}

void
ocad_file::write (const char * fn) const{
  // open file
  FILE * F = fopen(fn, "w");
  if (!F) throw "can't open file";

  ocad_header h;
  h.version = v;
  h.subversion = sv;
  h.type = t;

  h.seek(F);

  if (v<9) shead.write(F);
  if (v>8){
    h.fname_pos=fname.write(F);
    h.fname_size=(ftell(F)-h.fname_pos);
  }

  if (v>8){
    ocad9_objects o9;
    o9.insert(o9.end(), objects.begin(), objects.end());
    h.obj9_pos = o9.write(F, v);
  }
  else{
    ocad8_objects o8;
    o8.insert(o8.end(), objects.begin(), objects.end());
    h.obj8_pos = o8.write(F, v);
  }

  if (v>7) h.str_pos = strings.write(F, v);

  if (v>8){
    ocad9_symbols s9;
    s9.insert(s9.end(), symbols.begin(), symbols.end());
    h.sym_pos = s9.write(F, v);
  }
  else{
    ocad8_symbols s8;
    s8.insert(s8.end(), symbols.begin(), symbols.end());
    h.sym_pos = s8.write(F, v);
  }

  h.write(F);
}

void
ocad_file::update_extents(){
  ocad_objects::iterator o;
  for (o=objects.begin(); o!=objects.end(); o++){
    int sym = o->sym;
    ocad_symbols::const_iterator s;
    bool found=false;
    for (s=symbols.begin(); s!=symbols.end(); s++){
      if (s->sym == o->sym){
        o->extent = s->extent;
        if (o->type!=s->type)
           cerr << "warning: object ans symbol types differ: "
                     << o->type << " vs " << s->type << "\n";
        found=true;
      }
    }
    if (!found)
       cerr << "warning: no symbol: "
                 << o->sym << "\n";
  }
}

iRect
ocad_file::range() const{
  iRect ret;
  ocad_objects::const_iterator o;
  for (o=objects.begin(); o!=objects.end(); o++){
    ret = rect_bounding_box(ret, o->range());
  }
  return ret;
}

} // namespace
