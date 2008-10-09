#include <boost/spirit/core.hpp>
#include <boost/spirit/actor/assign_actor.hpp>
#include <boost/spirit/actor/push_back_actor.hpp>
#include <boost/spirit/actor/insert_at_actor.hpp>
#include <boost/spirit/actor/clear_actor.hpp>
#include <boost/spirit/actor/erase_actor.hpp>

#include "options.h"

namespace mapsoft{

using namespace boost::spirit;

bool Options::exists (const std::string & key) const {return (find(key) != end());}

std::set<std::string> Options::unknown (const std::set<std::string> & known) const{
    std::set<std::string> ret;
    for (const_iterator i=begin(); i!=end(); i++){
      if (known.count(i->first)==0) ret.insert(i->first);
    }
    return ret;
}

/** Операторы вывода-вывода для Options */

std::ostream & operator<< (std::ostream & s, const Options & o){
  for (Options::const_iterator i=o.begin(); i!=o.end(); i++){
    s << ((i==o.begin())?"":" ") << i->first << "=\"" << i->second << "\"";
  }
  return s;
}

std::istream & operator>> (std::istream & s, Options & o){

  std::string str1, str, aname, aval;
  while (!s.eof()){
    getline(s, str1);
    std::string::size_type pos = str1.find("#");
    str+= " " + ((pos==std::string::npos)? str1: str1.substr(0, pos));
  }

  // code from io_xml.cpp!
  rule<> attr_name = (+(alnum_p | '-' | '_'))[assign_a(aname)][assign_a(aval, "")];

  rule<> escaped_symb  = (ch_p('\\') >> ch_p('"')) | (ch_p('\\') >> ch_p('\\')) ;

  rule<> attr_value = ('"' >> (*((anychar_p | escaped_symb ) - '"'))[assign_a(aval)] >> '"') |
                      (*(anychar_p - (space_p | ch_p('"') | '>' | '/' | '\\')))[assign_a(aval)];

  rule<> attr = attr_name >> !(*space_p >> '=' >> *space_p >> attr_value);

  rule<> main_rule = *space_p >> *(attr[erase_a(o, aname)][insert_at_a(o, aname, aval)] >> *space_p);

  //
  parse_info<> res = parse(str.c_str(), main_rule);
  if (!res.full){
    std::cerr << "Can't parse options string " << str << " at pos " << res.length << "\n";
  }
  return s;
}


Options get_cmdline_options(int argc, char **argv);


}//namespace
