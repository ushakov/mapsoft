#include <boost/spirit/core.hpp>
#include <boost/spirit/actor/assign_actor.hpp>

#include "options.h"

namespace mapsoft{

std::set<std::string> Options::unknown (const std::set<std::string> & known) const{
    std::set<std::string> ret;
    for (const_iterator i=begin(); i!=end(); i++){
      if (known.count(i->first)==0) ret.insert(i->first);
    }
    return ret;
}

std::ostream & operator<< (std::ostream & s, const Options & o){
  for (Options::const_iterator i=o.begin(); i!=o.end(); i++){
    s << ((i==o.begin())?"":" ") << i->first << "=" << i->second;
  }
  return s;
}

}//namespace
