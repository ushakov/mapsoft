#include <iomanip>
#include <string>
#include <sstream>
#include <boost/spirit/core.hpp>
#include <boost/spirit/actor/assign_actor.hpp>

#include "m_color.h"

std::ostream & operator<< (std::ostream & s, const Color & t){
  s << "#" << std::setbase(16) << std::setw(8) 
    << std::setfill('0') << t.value << std::setbase(10);
  return s;
}

Color::Color(const std::string & str){
  using namespace boost::spirit;
  if (!parse(str.c_str(), ch_p('#') >> hex_p[assign_a(value)]).full)
    std::cerr << "Color: can't find hexadecimal value in " << str << "\n";
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

