#ifndef M_COLOR_H
#define M_COLOR_H

#include <iostream>
#include <boost/operators.hpp>

struct Color:
    public boost::equality_comparable<Color>,
    public boost::less_than_comparable<Color>
{
    int value;

    Color(){value=0xFF000000;}
    Color(const Color & t){value=t.value;}
    Color(int v){value=v;}

    Color(int alpha, int v){value=v + ((alpha &0xFF) << 24);}
    Color RGB() const {return Color(value & 0xFFFFFF);} 
    // чтоб, например в io_oe писать << color.RBG() 

  bool operator<  (const Color & t) const { return value < t.value; }
  bool operator== (const Color & t) const { return value == t.value; }
};

std::ostream & operator<< (std::ostream & s, const Color & t);
std::istream & operator>> (std::istream & s, Color & t);

#endif
