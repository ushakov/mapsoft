#ifndef M_COLOR_H
#define M_COLOR_H

#include <iostream>
#include <boost/operators.hpp>

struct Color:
    public boost::equality_comparable<Color>,
    public boost::less_than_comparable<Color>
{
    int value;

    Color(const Color & t):value(t.value){}
    Color(int v=0xFF000000):value(v){}
    Color(const std::string & s);

    Color(int alpha, int v){value=v + ((alpha &0xFF) << 24);}
    Color RGB() const {return Color(value & 0xFFFFFF);} 
    // чтоб, например в io_oe писать << color.RBG() 

  bool operator<  (const Color & t) const { return value < t.value; }
  bool operator== (const Color & t) const { return value == t.value; }
  operator std::string () const;
};

std::ostream & operator<< (std::ostream & s, const Color & t);
std::istream & operator>> (std::istream & s, Color & t);

#endif
