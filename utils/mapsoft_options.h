#include <boost/spirit/core.hpp>
#include <boost/spirit/actor/assign_actor.hpp>

#include <string>
#include <map>
#include <iostream>
#include <iomanip>
#include "mapsoft_time.h"



struct Options : std::map<std::string,std::string>{

  std::string get_string(const std::string & key, const std::string & dflt = "", bool _erase=false){
    iterator i = find(key);
    if (i == end() ) return dflt;

    std::string ret = i->second;
    if (_erase) erase(i);
    return ret;
  }


  double get_double(const std::string & key, const double dflt = 0.0, bool _erase=false){
    using namespace boost::spirit;
    iterator i = find(key);
    if (i == end() ) return dflt;

    std::string str = i->second;
    if (_erase) erase(i);

    double ret;
    if (parse(str.c_str(), real_p[assign_a(ret)]).full) return ret;

    std::cerr << "Options: can't find double value in " 
              << key << " = " << str
              << " Using default value: " << dflt << "\n"; 
    return dflt;
  }


  double get_udouble(const std::string & key, const double dflt = 0.0, bool _erase=false){
    using namespace boost::spirit;
    iterator i = find(key);
    if (i == end() ) return dflt;

    std::string str = i->second;
    if (_erase) erase(i);

    double ret;
    if (parse(str.c_str(), ureal_p[assign_a(ret)]).full) return ret;

    std::cerr << "Options: can't find unsigned double value in " 
              << key << " = " << str 
              << " Using default value: " << dflt << "\n"; 
    return dflt;
  }


  double get_int(const std::string & key, const int dflt = 0, bool _erase=false){
    using namespace boost::spirit;
    iterator i = find(key);
    if (i == end() ) return dflt;

    std::string str = i->second;
    if (_erase) erase(i);

    int ret;
    if (parse(str.c_str(), int_p[assign_a(ret)]).full) return ret;

    std::cerr << "Options: can't find int value in " 
	      << key << " = " << str
              << " Using default value: " << dflt << "\n"; 
    return dflt;
  }


  double get_uint(const std::string & key, const unsigned int dflt = 0, bool _erase=false){
    using namespace boost::spirit;
    iterator i = find(key);
    if (i == end() ) return dflt;

    std::string str = i->second;
    if (_erase) erase(i);

    unsigned int ret;
    if (parse(str.c_str(), uint_p[assign_a(ret)]).full) return ret;

    std::cerr << "Options: can't find unsigned int value in " 
	      << key << " = " << str
              << " Using default value: " << dflt << "\n"; 
    return dflt;
  }

  // hex values "#FFFFFF"
  double get_hex(const std::string & key, const unsigned int dflt = 0, bool _erase=false){
    using namespace boost::spirit;
    iterator i = find(key);
    if (i == end() ) return dflt;

    std::string str = i->second;
    if (_erase) erase(i);

    unsigned int ret;
    if (parse(str.c_str(), ch_p('#') >> hex_p[assign_a(ret)]).full) return ret;

    std::cerr << "Options: can't find hexadecimal value in " 
	      << key << " = " << str
              << " Using default value: #" << std::setbase(16) << dflt << "\n"; 
    return dflt;
  }

  // default value - current time. See ./time.h
  time_t get_time(const std::string & key, bool _erase=false){
    using namespace boost::spirit;
    iterator i = find(key);
    if (i == end() ) return time(NULL);

    std::string str = i->second;
    if (_erase) erase(i);

    return str2time(str);
  }
};


std::ostream & operator<< (std::ostream & s, const Options & o)
{
  s << "Options(" << "\n";
  for (Options::const_iterator i=o.begin(); i!=o.end(); i++){
    s << "  " << i->first << " = " << i->second << "\n";
  }
  s << ")\n";
  return s;
}
