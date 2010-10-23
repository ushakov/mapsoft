#ifndef OCAD_FNAME_H
#define OCAD_FNAME_H

#include <cstdio>
#include <string>

namespace ocad{

/// OCAD9 file name data block
struct ocad_fname {

  std::string data;

  void read(FILE *F, int addr, int size);

  int write(FILE *F) const;

  void dump(int verb);
};

} // namespace
#endif
