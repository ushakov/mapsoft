#ifndef OCAD_COLORS_H
#define OCAD_COLORS_H

#include <ios>
#include <sstream>
#include <iomanip>
#include "ocad_types.h"
#include "ocad_string.h"

namespace ocad{

struct ocad_cmyk{ // 4 bytes
  ocad_byte C,M,Y,K;   // 0-200?

  /// constructor: zero values
  ocad_cmyk(): C(0),M(0),Y(0),K(0){}

  // get 0xCCMMYYKK
  int get_int() const{
    return (C<<24) + (M<<16) + (Y<<8) + K;
  }

  // set from 0xCCMMYYBB
  void set_int(int c){
    C=(c>>24)&0xFF;
    M=(c>>16)&0xFF;
    Y=(c>>8)&0xFF;
    K=c&0xFF;
  }

  // get 0xRRGGBB -- INCORRECT?
  int get_rgb_int() const{
    int R = ((0xFF-C)*(0xFF-K))/0xFF;
    int G = ((0xFF-M)*(0xFF-K))/0xFF;
    int B = ((0xFF-Y)*(0xFF-K))/0xFF;
    return (R<<16) + (G<<8) + B;
  }

  // set from 0xRRGGBB -- INCORRECT?
  void set_rgb_int(int c){
    int R=(c>>16)&0xFF;
    int G=(c>>8)&0xFF;
    int B=c&0xFF;
    K = MIN(MIN(0xFF-R, 0xFF-G), 0xFF-B);
    if (K==0xFF) {
      C=M=Y=0;
    }
    else {
      C=(0xFF*(0xFF-R-K))/(0xFF-K);
      M=(0xFF*(0xFF-G-K))/(0xFF-K);
      Y=(0xFF*(0xFF-B-K))/(0xFF-K);
    }
  }

  // dump cmyk
  void dump_hex(std::ostream & s) const{
    s << "0x" << std::setw(8) << std::setfill('0') << std::setbase(16)
              << get_int()
              << std::setfill(' ') << std::setbase(10);
  }

  // dump rgb -- INCORRECT?
  void dump_rgb_hex(std::ostream & s) const{
    s << "0x" << std::setw(6) << std::setfill('0') << std::setbase(16)
              << get_rgb_int()
              << std::setfill(' ') << std::setbase(10);
  }
};

struct ocad_freq_ang{
  ocad_small f, a;
  ocad_freq_ang(): f(0), a(0){}
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

  ocad_colorinfo(): num(0), r1(0){
    memset(name, 0, sizeof(name));
    memset(sep_per, 0xFF, sizeof(sep_per));
  }

  ocad_string to_string() const{
    ocad_string ret;
    ret.type = 9;
    std::ostringstream s;
    s << name
      << "\tn" << num
      << "\tc" << color.C
      << "\tm" << color.M
      << "\ty" << color.Y
      << "\tk" << color.K
      << "\to0"   // overprint
      << "\tt100" // transparency
      ;
      // ... other fields?
    ret.data=s.str();
    return ret;
  }

  void from_string(const ocad_string & s){
    if (s.type!=9) return;
    *this = ocad_colorinfo();
    str_str2pas(name, s.get_field(0), 32);
    num = atoi(s.get_field('n').c_str());
    color.C = int(2*atof(s.get_field('c').c_str()));
    color.M = int(2*atof(s.get_field('m').c_str()));
    color.Y = int(2*atof(s.get_field('y').c_str()));
    color.K = int(2*atof(s.get_field('k').c_str()));
    //... other fields?
  }

  // dump_color
  void dump(std::ostream & s, int num_sep) const{
    s << std::setw(3) << num << " ";
    color.dump_hex(s);
    s << " [";
    dump_sep(s, num_sep);
    s << "]  \"" << str_pas2str(name, 32) << "\"";
  }

  // dump sep_par
  void dump_sep(std::ostream & s, int num_sep) const{
    s << std::setfill('0') << std::setbase(16);
    for (int i=0; i<MIN(num_sep, MAX_COLSEP); i++)
      s << " " << std::setw(2) << (int)sep_per[i];
    s << std::setfill(' ') << std::setbase(10);
  }

};

struct ocad_colorsep{ // 24 bytes
  char name[16];         // name of the color separation (pascal-string!)
  ocad_cmyk color;       // 0 in OCAD 6,
                         // CMYK value of the separation in OCAD 7.
                         // This value is only used in  the AI (Adobe Illustrator) export
  ocad_freq_ang raster;  // the halfton frequency and angle

  ocad_colorsep(){
    memset(name, 0, sizeof(name));
  }

  ocad_string to_string(int n) const{
    ocad_string ret;
    ret.type = 10;
    std::ostringstream s;
    s << name
      << "\tv1"
      << "\tn" << n
      << "\tf" << raster.f
      << "\ta" << raster.a/10.0
      << "\tc" << color.C/2.0
      << "\tm" << color.M/2.0
      << "\ty" << color.Y/2.0
      << "\tk" << color.K/2.0;
    ret.data=s.str();
    return ret;
  }
  void from_string(const ocad_string & s){
    if (s.type!=10) return;
    *this = ocad_colorsep();
    str_str2pas(name, s.get_field(0), 16);
    color.C = int(2*atof(s.get_field('c').c_str()));
    color.M = int(2*atof(s.get_field('m').c_str()));
    color.Y = int(2*atof(s.get_field('y').c_str()));
    color.K = int(2*atof(s.get_field('k').c_str()));
    raster.f = int(atof(s.get_field('f').c_str()));
    raster.a = int(10*atof(s.get_field('a').c_str()));
    //... other fields?
    // problem with number!
  }

  // dump color separation
  void dump(std::ostream & s) const{
    s << std::setw(4) << raster.f << "/" << raster.a << " ";
    color.dump_hex(s);
    s << " \"" << str_pas2str(name, 16) << "\"";
  }
};

} // namespace
#endif
