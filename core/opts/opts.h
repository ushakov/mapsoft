#ifndef OPT_H
#define OPT_H

#include <map>
#include <string>
#include <sstream>
#include <list>
#include "err/err.h"

struct Opts : public std::map<std::string,std::string>{

  /// Set option value for a given key
  template<typename T>
  void put (const std::string & key, T val) {
    std::ostringstream ss;
    ss << val;
    (*this)[key] = ss.str();
  }

  /// Returns value for given key
  /// If option does not exists or cast fails then default value is returned
  template<typename T>
  T get (const std::string & key, const T & def = T()) const {
    std::map<std::string, std::string>::const_iterator it = find(key);
    if (it == end()) return def;
    std::istringstream ss(it->second);
    T val;
    ss >> val;
    if (ss.fail() || !ss.eof())
      throw Err() << "can't parse value of " << key << ": " << it->second;
    return val;
  }

  // check if option exists
  bool exists (const std::string & key) const {return find(key) != end();}

  // find unknown options
  void check_unknown (std::list<std::string> known) const;
};

// String representation of Opts is a JSON object with
// string fields.

// input/output operators for options
std::ostream & operator<< (std::ostream & s, const Opts & o);
std::istream & operator>> (std::istream & s, Opts & o);


#endif
