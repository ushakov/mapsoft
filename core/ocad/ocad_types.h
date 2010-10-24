#ifndef OCAD_TYPES_H
#define OCAD_TYPES_H

#include <string>
#include "../../core/utils/iconv_utils.h"
namespace ocad{

#define MIN(x,y) ((x)<(y)?(x):(y))
#define MAX(x,y) ((x)>(y)?(x):(y))

/// convert pascal-string to c-string
/// maxlen includes first byte;
/// NOT USED?
void str_pas2c(char * str, int maxlen);

/// convert pascal-string to std::string
/// maxlen includes first byte;
std::string str_pas2str(const char * str, int maxlen);

/// convert std::string to pascal-string
///  maxlen includes first byte
void str_str2pas(char * pas, const std::string & str, int maxlen);

extern const IConv iconv_uni;
extern const IConv iconv_win;

/// OCAD6-7-8-9 data types

typedef int32_t  ocad_long;
typedef int32_t  ocad_int;
typedef int16_t  ocad_small;
typedef uint16_t ocad_word;
typedef uint16_t ocad_bool;
typedef uint8_t  ocad_byte;
typedef double   ocad_duble;

struct ocad_coord{ // 8 bytes
  ocad_long x;
  ocad_long y;

  ocad_coord();
  int getx() const;
  int gety() const;
  int getf() const;
  void setx(int v);
  void sety(int v);

  bool is_curve_f() const; ///<< this point is the first curve point
  bool is_curve_s() const; ///< this point is the second curve point
  bool is_curve() const;   ///< this point is curve point

/*
  /// for double lines: there is no left line between this point and the next point
  bool is_no_left() const {return fx & 4;}

  /// v9: this point is a area border line gap (?)
  bool v9_is_ablg() const {return fx & 8;}

  /// this point is a corner point
  bool is_corner() const {return fy & 1;}

  /// this point is the first point of a hole in an area
  bool is_hole_f() const {return fy & 2;}

  /// for double lines: there is no right line between this point and the next point
  bool is_no_right() const {return fy & 4;}

  /// OCAD 7, OCAD 9: this point is a dash point (and 8?)
  bool v79_is_dash() const {return fy & 8;}
*/
  void dump(std::ostream & s) const;
};

// protect against unsupported versions
void check_v(int * v);

} // namespace
#endif
