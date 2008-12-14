#include <iomanip>
#include <string>
#include <sstream>
#include <boost/spirit/core.hpp>
#include <boost/spirit/actor/assign_actor.hpp>

#include "m_color.h"

std::ostream & operator<< (std::ostream & s, const Color & t){
  s << "#" << std::setbase(16)
    << std::setw(2) << std::setfill('0') << (255 - ((t.value & 0xff000000) >> 24))
    << std::setw(6) << std::setfill('0') << (t.value & 0xffffff)
    << std::setbase(10);
  return s;
}

Color::Color(const std::string & str){
  using namespace boost::spirit;
  if (!parse(str.c_str(), ch_p('#') >> hex_p[assign_a(value)]).full)
    std::cerr << "Color: can't find hexadecimal value in " << str << "\n";
  int alpha = (value & 0xff000000) >> 24;
  value = value & 0xffffff;
  value |= (255 - alpha) << 24;
}

std::istream & operator>> (std::istream & s, Color & t){
  std::string str;
  std::getline(s, str);
  Color c(str);
  t.value=Color(str).value;
  return s;
}

Color::operator std::string () const{
  std::ostringstream o;
  o << *this;
  return o.str();
}

