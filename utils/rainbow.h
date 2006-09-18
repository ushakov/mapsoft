#ifndef RAINBOW_H
#define RAINBOW_H

#include <color.h>

inline RGB b_c(unsigned char x) {return RGB(0,x,255);}
inline RGB c_g(unsigned char x) {return RGB(0,255,255-x);}
inline RGB g_y(unsigned char x) {return RGB(x,255,0);}
inline RGB y_r(unsigned char x) {return RGB(255,255-x,0);}
inline RGB r_m(unsigned char x) {return RGB(255,0,x);}
inline RGB m_b(unsigned char x) {return RGB(255-x,0,255);}

// mingrade - чему соответствует n=0:
//  0 - blue, 1 - сyan, 2 - green etc.

RGB rainbow(int n, unsigned short mingrade) {
  switch ( (mingrade + n/256)%6 ){
    case 0: return b_c(n % 256);
    case 1: return c_g(n % 256);
    case 2: return g_y(n % 256);
    case 3: return y_r(n % 256);
    case 4: return r_m(n % 256);
    case 5: return m_b(n % 256);
  }
}

RGB rainbow(int n,
            int min,
            int max,
            int grades,
            int mingrade){
  if (n<min) n=min;
  if (n>max) n=max;

  return rainbow(((n-min)*256*grades)/(max-min), mingrade);
};

#endif
