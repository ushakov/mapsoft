#include <iomanip>
#include <string>
#include <boost/spirit/core.hpp>
#include <boost/spirit/actor/assign_actor.hpp>

#include "m_color.h"

std::ostream & operator<< (std::ostream & s, const Color & t){
  s << "#" << std::setbase(16) << std::setw(8) 
    << std::setfill('0') << t.value << std::setbase(10);
  return s;
}
std::istream & operator>> (std::istream & s, Color & t){
  using namespace boost::spirit;
  std::string str;
  std::getline(s, str);
  if (!parse(str.c_str(), ch_p('#') >> hex_p[assign_a(t.value)]).full)
    std::cerr << "Color: can't find hexadecimal value in " << str << "\n";
  return s;
}
