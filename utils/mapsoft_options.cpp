#include <boost/spirit/core.hpp>
#include <boost/spirit/actor/assign_actor.hpp>

#include <iostream>
#include <iomanip>
#include "mapsoft_time.h"
#include "mapsoft_options.h"


std::string  Options::get_string  (const std::string & key, const std::string & dflt ) const {
  const_iterator i = find(key);
  if (i == end() ) return dflt;
  else return i->second;
}

double Options::get_double  (const std::string & key, const double dflt) const {
  using namespace boost::spirit;
  const_iterator i = find(key);
  if (i == end() ) return dflt;

  double ret;
  if (parse(i->second.c_str(), real_p[assign_a(ret)]).full) return ret;
  std::cerr << "Options: can't find double value in " 
            << key << " = " << i->second
            << " Using default value: " << dflt << "\n"; 
  return dflt;
}

double Options::get_udouble (const std::string & key, const double dflt) const {
  using namespace boost::spirit;
  const_iterator i = find(key);
  if (i == end() ) return dflt;

  double ret;
  if (parse(i->second.c_str(), ureal_p[assign_a(ret)]).full) return ret;
  std::cerr << "Options: can't find unsigned double value in " 
            << key << " = " << i->second 
            << " Using default value: " << dflt << "\n"; 
  return dflt;
}

int Options::get_int (const std::string & key, const int dflt) const {
  using namespace boost::spirit;
  const_iterator i = find(key);
  if (i == end() ) return dflt;

  int ret;
  if (parse(i->second.c_str(), int_p[assign_a(ret)]).full) return ret;
  std::cerr << "Options: can't find int value in " 
            << key << " = " << i->second
            << " Using default value: " << dflt << "\n"; 
  return dflt;
}

unsigned int Options::get_uint (const std::string & key, const unsigned int dflt) const {
  using namespace boost::spirit;
  const_iterator i = find(key);
  if (i == end() ) return dflt;

  unsigned int ret;
  if (parse(i->second.c_str(), uint_p[assign_a(ret)]).full) return ret;
  std::cerr << "Options: can't find unsigned int value in " 
            << key << " = " << i->second
            << " Using default value: " << dflt << "\n"; 
  return dflt;
}

char Options::get_char (const std::string & key, const char dflt) const {
  const_iterator i = find(key);
  if (i == end() ) return dflt;

  if (i->second.length()==1) return i->second[0];
  std::cerr << "Options: can't find char value in " 
            << key << " = " << i->second
            << " Using default value: " << dflt << "\n"; 
  return dflt;
}

// hex values "#FFFFFF"
unsigned int Options::get_hex (const std::string & key, const unsigned int dflt) const {
  using namespace boost::spirit;
  const_iterator i = find(key);
  if (i == end() ) return dflt;

  unsigned int ret;
  if (parse(i->second.c_str(), ch_p('#') >> hex_p[assign_a(ret)]).full) return ret;
  std::cerr << "Options: can't find hexadecimal value in " 
            << key << " = " << i->second
            << " Using default value: #" << std::setbase(16) << dflt << "\n"; 
  return dflt;
}

bool Options::get_bool(const std::string & key) const {
  return (find(key) != end());
}

// default value - current time. See ./mapsoft_time.h
time_t Options::get_time(const std::string & key, const time_t dflt) const {
  using namespace boost::spirit;
  const_iterator i = find(key);
  if (i == end() ) return dflt;
  return str2time(i->second);
}

std::vector<Point<double> > Options::get_poly 
    (const std::string & key, const std::vector<Point<double> > & dflt) const {
  using namespace boost::spirit;
  const_iterator i = find(key);
  if (i == end() ) return dflt;

  std::string str = "," + i->second;

  Point<double> pt;
  std::vector<Point<double> > ret;

  if (parse(str.c_str(), *(
          *space_p >> ',' >>  *space_p >>
          real_p[assign_a(pt.x)] >>
          *space_p >> ',' >>  *space_p >>
          real_p[assign_a(pt.y)][push_back_a(ret,pt)])).full) return ret;
    std::cerr << "Options: can't find polyline (comma sep. doubles) in "
              << key << " = " << i->second << "\n";
    return dflt;
}

void Options::warn_unused (const std::string * used) const{
    const_iterator i;
    for (i=begin(); i!=end(); i++){
	const std::string * str = &used[0];
	bool find = false;
	while (str->length()!=0){
	    if (*str == i->first) find = true;
	    str++;
	}
	if (!find)
	    std::cerr << "Unknown option: " 
                      << i->first << " = " << i->second << "\n";
    }
    return;
}


std::ostream & operator<< (std::ostream & s, const Options & o)
{
  s << "Options(" << "\n";
  for (Options::const_iterator i=o.begin(); i!=o.end(); i++){
    s << "  " << i->first << " = " << i->second << "\n";
  }
  s << ")\n";
  return s;
}
