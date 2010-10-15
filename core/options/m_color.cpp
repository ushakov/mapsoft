#include <iomanip>
#include <string>
#include <sstream>
#include <boost/spirit/include/classic_core.hpp>
#include <boost/spirit/include/classic_assign_actor.hpp>

#include "m_color.h"

std::ostream & operator<< (std::ostream & s, const Color & t){
  s << "#" << std::setbase(16)
    << std::setw(2) << std::setfill('0') << (255 - ((t.value & 0xff000000) >> 24))
    << std::setw(2) << std::setfill('0') << (t.value & 0xff)              // R
    << std::setw(2) << std::setfill('0') << ((t.value & 0xff00) >> 8)     // G
    << std::setw(2) << std::setfill('0') << ((t.value & 0xff0000) >> 16)  // B
    << std::setbase(10);
  return s;
}

Color::Color(const std::string & str){
  using namespace boost::spirit::classic;
  if (!parse(str.c_str(), ch_p('#') >> hex_p[assign_a(value)]).full)
    std::cerr << "Color: can't find hexadecimal value in " << str << "\n";
  int alpha = (value & 0xff000000) >> 24;
  int r = (value & 0xff0000) >> 16;
  int g = (value & 0xff00) >> 8;
  int b = value & 0xff;
  int rgb = (b << 16) | (g << 8) | r;
  value = ((255 - alpha) << 24) | rgb;
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

