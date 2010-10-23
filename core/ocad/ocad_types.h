#ifndef OCAD_TYPES_H
#define OCAD_TYPES_H

#include <iostream>
#include <iomanip>
#include <string>
#include <cassert>
#include "../../core/utils/iconv_utils.h"
namespace ocad{

#define MIN(x,y) ((x)<(y)?(x):(y))
#define MAX(x,y) ((x)>(y)?(x):(y))

/// convert pascal-string to c-string
/// maxlen includes first byte;
/// NOT USED?
void str_pas2c(char * str, int maxlen){
  int l = str[0];
  if (l>=maxlen){
    std::cerr << "warning: not a pascal-string?\n";
    l=maxlen-1;
  }
  for (int i=0; i<l; i++) str[i]=str[i+1];
  str[l]='\0';
}

/// convert pascal-string to std::string
/// maxlen includes first byte;
std::string str_pas2str(const char * str, int maxlen){
  int l = str[0]+1;
  if (l>maxlen){
    std::cerr << "warning: not a pascal-string? "
              << "(first byte: " << (int)str[0] << ")\n";
    l=maxlen;
  }
  if (l<1){
    std::cerr << "warning: not a pascal-string? "
              << "(first byte: " << (int)str[0] << ")\n";
    l=1;
  }
  return std::string(str+1, str+l);
}

/// convert std::string to pascal-string
///  maxlen includes first byte
void
str_str2pas(char * pas, const std::string & str, int maxlen){
  if (str.size() > maxlen-1)
    std::cerr << "warning: cropping string.";

  int size = MIN(str.size(), maxlen-1);
  pas[0] = size;
  for (int i=0; i<size; i++) pas[i+1]=str[i];
}

IConv iconv_uni("UTF-16");
IConv iconv_win("CP1251");

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

  ocad_coord(){
     assert ( sizeof(*this) == 8);
    memset(this, 0, sizeof(*this));
  }

  int getx() const{
    return x>>8;
  }
  int gety() const{
    return y>>8;
  }
  int getf() const{
    return (x & 0xFF) << 8 + (y & 0xFF);
  }
  void setx(int v){
    x = (x & 0xFF) + v << 8;
  }
  void sety(int v){
    y = (y & 0xFF) + v << 8;
  }

/*
  /// this point is the first curve point
  bool is_curve_f() const {return fx & 1;}

  /// this point is the second curve point
  bool is_curve_s() const {return fx & 2;}

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
  void dump(std::ostream & s) const{
    s << " " << getx() << "," << gety();
    if (getf()) s << "(" << std::setbase(16) << getf()
                         << std::setbase(10) << ")";
  }
};

// protect against unsupported versions
void check_v(int * v){
  if ((*v<6)||(*v>9)){
     std::cerr << "unsupported version: " << *v << ", set to 9\n";
     *v=9;
  }
}

} // namespace
#endif
