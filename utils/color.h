#ifndef COLOR_H
#define COLOR_H

namespace Color{

struct RGB{
    unsigned char r,g,b;

    RGB(): r(0),g(0),b(0){}

    RGB(unsigned char _r, unsigned char _g, unsigned char _b):
	r(_r), g(_g), b(_b) {}

    RGB(int c):
	r((c>>24)&0xFF), g((c>>16)&0xFF), b((c>>8)&0xFF) {}

//    RGB(const RGBA & c):
//	r(c.r), g(c.g), b(c.b) {}

    operator int () const{
      return (int(r)<<24) + (int(g)<<16) + (int(b)<<8);
    }

};

struct RGBA{
    unsigned char r,g,b,a;

    RGBA(): r(0),g(0),b(0),a(0){}

    RGBA(unsigned char _r, unsigned char _g, unsigned char _b, unsigned char _a):
	r(_r), g(_g), b(_b), a(_a) {}

    RGBA(int c):
	r((c>>24)&0xFF), g((c>>16)&0xFF), b((c>>8)&0xFF), a(c&0xFF) {}

    RGBA(const RGB & c):
	r(c.r), g(c.g), b(c.b), a(0) {}

    operator int () const{
      return (int(r)<<24) + (int(g)<<16) + (int(b)<<8) + a;
    }

};

std::ostream & operator<< (std::ostream & s, const RGB & c)
{
  s << "RGB(" 
    << (int)c.r << "," 
    << (int)c.g << "," 
    << (int)c.b << ")";
  return s;
}

std::ostream & operator<< (std::ostream & s, const RGBA & c)
{
  s << "RGBA(" 
    << (int)c.r << "," 
    << (int)c.g << "," 
    << (int)c.b << "," 
    << (int)c.a << ")";
  return s;
}

}//namespace
#endif
