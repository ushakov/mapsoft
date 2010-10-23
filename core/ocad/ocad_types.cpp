#include <iostream>
#include <iomanip>
#include <string>
#include <cassert>
#include "ocad_types.h"
#include "utils/iconv_utils.h"

using namespace std;

namespace ocad{

#define MIN(x,y) ((x)<(y)?(x):(y))
#define MAX(x,y) ((x)>(y)?(x):(y))

void
str_pas2c(char * str, int maxlen){
  int l = str[0];
  if (l>=maxlen){
    cerr << "warning: not a pascal-string?\n";
    l=maxlen-1;
  }
  for (int i=0; i<l; i++) str[i]=str[i+1];
  str[l]='\0';
}

string
str_pas2str(const char * str, int maxlen){
  int l = str[0]+1;
  if (l>maxlen){
    cerr << "warning: not a pascal-string? "
              << "(first byte: " << (int)str[0] << ")\n";
    l=maxlen;
  }
  if (l<1){
    cerr << "warning: not a pascal-string? "
              << "(first byte: " << (int)str[0] << ")\n";
    l=1;
  }
  return string(str+1, str+l);
}

void
str_str2pas(char * pas, const string & str, int maxlen){
  if (str.size() > maxlen-1)
    cerr << "warning: cropping string.";

  int size = MIN(str.size(), maxlen-1);
  pas[0] = size;
  for (int i=0; i<size; i++) pas[i+1]=str[i];
}

const IConv iconv_uni("UTF-16");
const IConv iconv_win("CP1251");

ocad_coord::ocad_coord(){
  assert ( sizeof(*this) == 8);
  memset(this, 0, sizeof(*this));
}

int
ocad_coord::getx() const{
  return x>>8;
}
int
ocad_coord::gety() const{
  return y>>8;
}
int
ocad_coord::getf() const{
  return (x & 0xFF) << 8 + (y & 0xFF);
}
void
ocad_coord::setx(int v){
  x = (x & 0xFF) + v << 8;
}
void
ocad_coord::sety(int v){
  y = (y & 0xFF) + v << 8;
}

void
ocad_coord::dump(ostream & s) const{
  s << " " << getx() << "," << gety();
  if (getf()) s << "(" << setbase(16) << getf()
                       << setbase(10) << ")";
}

void
check_v(int * v){
  if ((*v<6)||(*v>9)){
     cerr << "unsupported version: " << *v << ", set to 9\n";
     *v=9;
  }
}

} // namespace
