#include <iostream>
#include "ocad_symbol.h"

using namespace std;

namespace ocad{

ocad_symbol::ocad_symbol():
  sym(0), type(0), extent(0), blob_version(0){}

void
ocad_symbol::dump(int verb) const{
  if (verb<1) return;
  cout
    << " sym: " << sym/1000 << "." << sym%1000
    << " type: " << (int)type
    << " extent: " << extent
    << " size: " << blob.size()
    << " version: " << blob_version
    << " \"" << desc
    << "\"\n";
  // ...
}


} // namespace
