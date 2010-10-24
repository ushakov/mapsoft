#ifndef OCAD8_SHEAD_H
#define OCAD8_SHEAD_H

#include <cstdio>
#include <cassert>
#include <vector>
#include "ocad_types.h"
#include "ocad_colors.h"
#include "ocad_string.h"


/// OCAD 6..8 symbol header. Information about colors and color separations.
/// In v9 all this data is kept in ocad_strings.

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

  /// Read from file.
  void read(FILE * F);

  /// Write to file.
  void write(FILE * F) const;

  /// Convert to strings (inaccurate).
  std::vector<ocad_string> to_strings() const;

  /// Set from strings (inaccurate).
  void from_strings(const std::vector<ocad_string> & strings);

  /// Dump all information to stdout.
  void dump(int verb) const;

};

} // namespace
#endif
