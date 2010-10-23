#ifndef OCAD8_SHEAD_H
#define OCAD8_SHEAD_H

#include <cstdio>
#include <vector>
#include "ocad_types.h"
#include "ocad_colors.h"

namespace ocad{

struct _ocad8_shead{ // 24 + 256*72 + 32*24 = 19224 bytes
  ocad_small ncolors;     // number of colors defined
  ocad_small ncolsep;     // number of color separations defined
  ocad_freq_ang C,M,Y,K;  // halftone frequency and angle of CMYK colors
  ocad_small r1,r2;
  ocad_colorinfo cols[256];
  ocad_colorsep  seps[32]; /// only 24 used
  _ocad8_shead(){
    assert(sizeof(*this) == 19224);
    memset(this, 0, sizeof(*this));
  }
};

struct ocad8_shead : _ocad8_shead{

  void read(FILE * F){
    _ocad8_shead h;
    if (fread(&h, 1, sizeof(h), F) != sizeof(h))
      throw "error while reading sheader";
    this->_ocad8_shead::operator=(h);
  }

  void write(FILE * F) const{
    _ocad8_shead h(*this);
    if (fwrite(&h, 1, sizeof(h), F) != sizeof(h))
      throw "error while writing symbol header";
  }

  std::vector<ocad_string> to_strings() const{
    std::vector<ocad_string> ret;
    for (int i=0; i<MIN(ncolors, 256); i++)
      ret.push_back(cols[i].to_string());
    for (int i=0; i<MIN(ncolsep, MAX_COLSEP); i++)
      ret.push_back(seps[i].to_string(i));
    return ret;
  }

  void from_strings(const std::vector<ocad_string> & strings){
    * this = ocad8_shead(); // reset data
    std::vector<ocad_string>::const_iterator s;
    ncolors=0;
    ncolsep=0;
    for (s=strings.begin(); s!=strings.end(); s++){
      if (s->type == 9){
        if (ncolors<256) cols[ncolors++].from_string(*s);
        else "warning: too many colors";
      }
      if (s->type == 10){
        if (ncolsep<MAX_COLSEP) seps[ncolsep++].from_string(*s);
        else "warning: too many colsep";
      }
    }
  }

  void dump(int verb) const{
    if (verb<1) return;
    std::cout
      << "Symbol header block: "
      << ncolors << " colors, "
      << ncolsep << " color separations\n";
    if (verb<2) return;
    std::cout
      << "halftone frequency/angle:\n"
      << " C: " << C.f << "/" << C.a << "\n"
      << " M: " << C.f << "/" << C.a << "\n"
      << " Y: " << C.f << "/" << C.a << "\n"
      << " K: " << C.f << "/" << C.a << "\n"
      << "colors:\n";
    for (int i=0; i<MIN(ncolors, 256); i++){
      std::cout << " ";
      cols[i].dump(std::cout, ncolsep);
      std::cout << "\n";
    }
    std::cout << "color separations:\n";
    for (int i=0; i<MIN(ncolsep, MAX_COLSEP); i++){
      std::cout << " ";
      seps[i].dump(std::cout);
      std::cout << "\n";
    }
  }

};

} // namespace
#endif
