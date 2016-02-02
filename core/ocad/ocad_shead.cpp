#include "ocad_shead.h"
#include <iostream>

using namespace std;

namespace ocad{

void
ocad8_shead::read(FILE * F){
  _ocad8_shead h;
  if (fread(&h, 1, sizeof(h), F) != sizeof(h))
    throw "error while reading sheader";
  this->_ocad8_shead::operator=(h);
}

void
ocad8_shead::write(FILE * F) const{
  _ocad8_shead h(*this);
  if (fwrite(&h, 1, sizeof(h), F) != sizeof(h))
    throw "error while writing symbol header";
}

vector<ocad_string>
ocad8_shead::to_strings() const{
  vector<ocad_string> ret;
  for (int i=0; i<std::min(ncolors, (ocad_small)256); i++)
    ret.push_back(cols[i].to_string());
  for (int i=0; i<std::min(ncolsep, (ocad_small)MAX_COLSEP); i++)
    ret.push_back(seps[i].to_string(i));
  return ret;
}

void
ocad8_shead::from_strings(const vector<ocad_string> & strings){
  * this = ocad8_shead(); // reset data
  vector<ocad_string>::const_iterator s;
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

void
ocad8_shead::dump(int verb) const{
  if (verb<1) return;
  cout
    << "Symbol header block: "
    << ncolors << " colors, "
    << ncolsep << " color separations\n";
  if (verb<2) return;
  cout
    << "halftone frequency/angle:\n"
    << " C: " << C.f << "/" << C.a << "\n"
    << " M: " << C.f << "/" << C.a << "\n"
    << " Y: " << C.f << "/" << C.a << "\n"
    << " K: " << C.f << "/" << C.a << "\n"
    << "colors:\n";
  for (int i=0; i<std::min(ncolors, (ocad_small)256); i++){
    cout << " ";
    cols[i].dump(cout, ncolsep);
    cout << "\n";
  }
  cout << "color separations:\n";
  for (int i=0; i<std::min(ncolsep, (ocad_small)MAX_COLSEP); i++){
    cout << " ";
    seps[i].dump(cout);
    cout << "\n";
  }
}

} // namespace
