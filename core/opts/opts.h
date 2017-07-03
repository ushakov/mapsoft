#ifndef OPT_H
#define OPT_H

#include <map>
#include <string>
#include <sstream>
#include <list>
#include <algorithm>
#include "err/err.h"
#include "jansson.h"

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
  void check_unknown (std::list<std::string> known) const{
    std::string unknown;
    int n=0;
    for (auto i : *this){
      if (std::find(known.begin(), known.end(), i.first) == known.end())
        unknown += (n++ ? ", ": " ") + i.first;
    }
    if (n){
      throw Err() << "unknown "
                  << (n==1? "option:":"options:")
                  << unknown;
    }
  }
};

// String representation of Opts is a JSON object with
// string fields.

// input/output operators for options
std::ostream & operator<< (std::ostream & s, const Opts & o){
  json_error_t e;
  json_t *J = json_object();
  for (auto i: o){
    json_object_set(J, i.first.c_str(), json_string(i.second.c_str()));
  }
  char *ret = json_dumps(J, JSON_SORT_KEYS);
  json_decref(J);
  if (!ret) throw Err() << "can't write Opts";
  s<<ret;
  free(ret);
  return s;
}

std::istream & operator>> (std::istream & s, Opts & o){

  // read the whole stream into a string
  std::ostringstream os;
  s>>os.rdbuf();
  std::string str=os.str();

  json_error_t e;
  json_t *J = json_loadb(str.data(), str.size(), 0, &e);

  o.clear(); // clear old contents
  try {
    if (!J) throw Err() << e.text;
    if (!json_is_object(J)) throw Err() << "a JSON object with string fields expected";

    const char *k;
    json_t *v;
    json_object_foreach(J, k, v){
      if (!json_is_string(v)) throw Err() << "a JSON object with string fields expected";
      o[k] = json_string_value(v);
    }
  }
  catch (Err e){
    json_decref(J);
    throw e;
  }
  json_decref(J);
  return s;
}


#endif
