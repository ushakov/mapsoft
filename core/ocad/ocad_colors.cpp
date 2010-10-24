#include <sstream>
#include <iomanip>
#include "ocad_colors.h"

using namespace std;

namespace ocad{

ocad_cmyk::ocad_cmyk(): C(0),M(0),Y(0),K(0){}

int
ocad_cmyk::get_int() const{
  return (C<<24) + (M<<16) + (Y<<8) + K;
}

void
ocad_cmyk::set_int(int c){
  C=(c>>24)&0xFF;
  M=(c>>16)&0xFF;
  Y=(c>>8)&0xFF;
  K=c&0xFF;
}

int
ocad_cmyk::get_rgb_int() const{
  int R = ((0xFF-C)*(0xFF-K))/0xFF;
  int G = ((0xFF-M)*(0xFF-K))/0xFF;
  int B = ((0xFF-Y)*(0xFF-K))/0xFF;
  return (R<<16) + (G<<8) + B;
}

void
ocad_cmyk::set_rgb_int(int c){
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

void
ocad_cmyk::dump_hex(ostream & s) const{
  s << "0x" << setw(8) << setfill('0') << setbase(16)
            << get_int()
            << setfill(' ') << setbase(10);
}

void
ocad_cmyk::dump_rgb_hex(ostream & s) const{
  s << "0x" << setw(6) << setfill('0') << setbase(16)
            << get_rgb_int()
            << setfill(' ') << setbase(10);
}

/***************************************/

ocad_freq_ang::ocad_freq_ang(): f(0), a(0){}

/***************************************/

ocad_colorinfo::ocad_colorinfo(): num(0), r1(0){
  memset(name, 0, sizeof(name));
  memset(sep_per, 0xFF, sizeof(sep_per));
}

ocad_string
ocad_colorinfo::to_string() const{
  ocad_string ret;
  ret.type = 9;
  ostringstream s;
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

void
ocad_colorinfo::from_string(const ocad_string & s){
  if (s.type!=9) return;
  *this = ocad_colorinfo();
  str_str2pas(name, s.get_str(0), 32);
  num = atoi(s.get_str('n').c_str());
  color.C = int(2*s.get<double>('c'));
  color.M = int(2*s.get<double>('m'));
  color.Y = int(2*s.get<double>('y'));
  color.K = int(2*s.get<double>('k'));
  //... other fields?
}

void
ocad_colorinfo::dump(ostream & s, int num_sep) const{
  s << setw(3) << num << " ";
  color.dump_hex(s);
  s << " [";
  dump_sep(s, num_sep);
  s << "]  \"" << str_pas2str(name, 32) << "\"";
}

void
ocad_colorinfo::dump_sep(ostream & s, int num_sep) const{
  s << setfill('0') << setbase(16);
  for (int i=0; i<MIN(num_sep, MAX_COLSEP); i++)
    s << " " << setw(2) << (int)sep_per[i];
  s << setfill(' ') << setbase(10);
}

/***************************************/

ocad_colorsep::ocad_colorsep(){
  memset(name, 0, sizeof(name));
}

ocad_string
ocad_colorsep::to_string(int n) const{
  ocad_string ret;
  ret.type = 10;
  ostringstream s;
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

void
ocad_colorsep::from_string(const ocad_string & s){
  if (s.type!=10) return;
  *this = ocad_colorsep();
  str_str2pas(name, s.get_str(0), 16);
  color.C =   int(2*s.get<double>('c'));
  color.M =   int(2*s.get<double>('m'));
  color.Y =   int(2*s.get<double>('y'));
  color.K =   int(2*s.get<double>('k'));
  raster.f =    int(s.get<double>('f'));
  raster.a = int(10*s.get<double>('a'));
  //... other fields?
  // problem with number!
}

void
ocad_colorsep::dump(ostream & s) const{
  s << setw(4) << raster.f << "/" << raster.a << " ";
  color.dump_hex(s);
  s << " \"" << str_pas2str(name, 16) << "\"";
}


} // namespace
