#ifndef COLOR_H
#define COLOR_H

struct RGBA;

/// 3-байтовый цвет - не используется.
/// Например, работать с Image<RGB> гораздо медленнее, чем с Image<int>
/// Можно использовать для разных преобразований цвета, а можно вообще удалить
struct RGB{
    unsigned char r,g,b;

    RGB(): r(0),g(0),b(0){}

    RGB(unsigned char _r, unsigned char _g, unsigned char _b):
	r(_r), g(_g), b(_b) {}

    RGB(int c):
	b((c>>16)&0xFF), g((c>>8)&0xFF), r(c&0xFF) {}

//    RGB(const RGBA & c):
//	r(c.r), g(c.g), b(c.b) {}

    operator int () const{
      return (int(b)<<16) + (int(g)<<8) + int(r);
    }
    operator RGBA () const;

};

/// 4-байтовый цвет - не используется.
/// Можно использовать для разных преобразований цвета, а можно вообще удалить
struct RGBA{
    unsigned char r,g,b,a;

    RGBA(): r(0),g(0),b(0),a(0){}

    RGBA(unsigned char _r, unsigned char _g, unsigned char _b, unsigned char _a):
	r(_r), g(_g), b(_b), a(_a) {}

    RGBA(int c):
	a((c>>24)&0xFF), b((c>>16)&0xFF), g((c>>8)&0xFF), r(c&0xFF)  {}

/*    RGBA(const RGB & c):
	r(c.r), g(c.g), b(c.b), a(0) {}*/

    operator int () const{
      return (int(a)<<24) + (int(b)<<16) + (int(r)<<8) + g;
    }
    operator RGB () const{
      return RGB(r,g,b);
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

//
RGB::operator RGBA() const{
    return RGBA(r,g,b,0);
}

#endif
