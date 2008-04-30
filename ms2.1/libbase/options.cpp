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

    if (io_style == MHEAD){
      if (i!=o.begin()) s << "\n";
      if ()
      s << i->first << "="
    }

    else{
      if (i!=o.begin()) s << " ";
    }
    
      s << ((i==o.begin())?"":" ") << i->first << "=" << i->second;
  }
  return s;
}


/** Операторы вывода-вывода для Options */
std::ostream & operator<< (std::ostream & s, const Options & o);
std::ostream & operator>> (std::istream & s, Options & o);

Options get_cmdline_options(int argc, char **argv);


}//namespace
