#ifndef COLOR_H
#define COLOR_H

struct color{
  unsigned char r,g,b;
  
  color(unsigned char _r,
        unsigned char _g,
        unsigned char _b)
  {r=_r; g=_g; b=_b;}

};

color red()   {return color(255,0,0);}
color green() {return color(0,255,0);}
color blue()  {return color(0,0,255);}

color cyan()    {return color(0,255,255);}
color magenta() {return color(255,0,255);}
color yellow()  {return color(255,255,0);}

color b_c(unsigned char x) {return color(0,x,255);}
color c_g(unsigned char x) {return color(0,255,255-x);}
color g_y(unsigned char x) {return color(x,255,0);}
color y_r(unsigned char x) {return color(255,255-x,0);}
color r_m(unsigned char x) {return color(255,0,x);}
color m_b(unsigned char x) {return color(255-x,0,255);}

// mingrade - чему соответствует n=0:
//  0 - blue, 1 - сyan, 2 - green etc.
color rainbow(int n, unsigned short mingrade) {
  switch ( (mingrade + n/256)%6 ){ 
    case 0: return b_c(n % 256);
    case 1: return c_g(n % 256);
    case 2: return g_y(n % 256);
    case 3: return y_r(n % 256);
    case 4: return r_m(n % 256);
    case 5: return m_b(n % 256);
  }
}

color rainbow(int n, 
              int min,
              int max,
              int grades,
              int mingrade){
  if (n<min) n=min;
  if (n>max) n=max;

  return rainbow(((n-min)*256*grades)/(max-min), mingrade);
};

color operator+ (color c1, color c2) 
  {return color(c1.r+c2.r, c1.g+c2.g, c1.b+c2.b);}

color operator* (color c1, double d){
  return color((unsigned char)(c1.r*d), 
               (unsigned char)(c1.g*d), 
               (unsigned char)(c1.b*d));
}


#endif
