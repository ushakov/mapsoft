#ifndef OCAD_COLORS_H
#define OCAD_COLORS_H

#include <ios>
#include "ocad_types.h"
#include "ocad_string.h"

namespace ocad{

struct ocad_cmyk{ // 4 bytes
  ocad_byte C,M,Y,K;   // 0-200?

  /// constructor: zero values
  ocad_cmyk();

  /// Get 0xCCMMYYKK.
  int get_int() const;

  /// Set from 0xCCMMYYBB.
  void set_int(int c);

  // get 0xRRGGBB -- INCORRECT?
  int get_rgb_int() const;

  // set from 0xRRGGBB -- INCORRECT?
  void set_rgb_int(int c);

  // dump cmyk
  void dump_hex(std::ostream & s) const;

  // dump rgb -- INCORRECT?
  void dump_rgb_hex(std::ostream & s) const;
};

struct ocad_freq_ang{
  ocad_small f, a;
  ocad_freq_ang();
};

#define MAX_COLSEP 24

struct ocad_colorinfo{ // 72 bytes
  ocad_small num;        // this number is used in the symbols when referring a color
  ocad_small r1;
  ocad_cmyk color;       // color value
  char name[32];         // description of the color (pascal-string)
  ocad_byte sep_per[32]; // Definition how the color appears in the different.
                         // spot color separations: 0..200: 2 times the separation percentage
                         // 255: the color does not appear in the corresponding color separation

  /// Zero constructor.
  ocad_colorinfo();

  /// Convert to ocad string (inaccurate yet)
  ocad_string to_string() const;

  /// Set from ocad string (inaccurate yet)
  void from_string(const ocad_string & s);

  /// Dump color to s. num_sep is a number of color separations.
  void dump(std::ostream & s, int num_sep) const;

  /// Dump single color separation info for color.
  /// Used in dump.
  void dump_sep(std::ostream & s, int num_sep) const;

};

struct ocad_colorsep{ // 24 bytes
  char name[16];         // name of the color separation (pascal-string!)
  ocad_cmyk color;       // 0 in OCAD 6,
                         // CMYK value of the separation in OCAD 7.
                         // This value is only used in  the AI (Adobe Illustrator) export
  ocad_freq_ang raster;  // the halfton frequency and angle

  /// Zero constructor.
  ocad_colorsep();

  /// Convert to ocad string (inaccurate yet)
  ocad_string to_string(int n) const;

  /// Set from ocad string (inaccurate yet)
  void from_string(const ocad_string & s);

  // Dump color separation to s.
  void dump(std::ostream & s) const;
};

} // namespace
#endif
