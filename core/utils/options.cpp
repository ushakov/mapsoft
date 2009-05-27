#include <boost/spirit/core.hpp>
#include <boost/spirit/actor/assign_actor.hpp>
#include <boost/spirit/actor/push_back_actor.hpp>
#include <boost/spirit/actor/insert_at_actor.hpp>
#include <boost/spirit/actor/clear_actor.hpp>
#include <boost/spirit/actor/erase_actor.hpp>

#include "options.h"

#include <ios>


using namespace boost::spirit;

bool Options::exists (const std::string & key) const {return (find(key) != end());}

/*
std::set<std::string> Options::unknown (const std::set<std::string> & known) const{
    std::set<std::string> ret;
    for (const_iterator i=begin(); i!=end(); i++){
      if (known.count(i->first)==0) ret.insert(i->first);
    }
    return ret;
}*/

void Options::warn_unused (const std::string * used) const{
  const_iterator i;
  for (i=begin(); i!=end(); i++){
    const std::string * str = &used[0];
    bool find = false;
    while (str->length()!=0){
      if (*str == i->first ) find = true;
      str++;
    }
    if (!find)
      std::cerr << "Unknown option: "
                << i->first << " = " << i->second << "\n";
  }
  return;
}


/** Операторы вывода-вывода для Options */

std::ostream & operator<< (std::ostream & s, const Options & o){
  for (Options::const_iterator i=o.begin(); i!=o.end(); i++){
    s << ((i==o.begin())?"":"\n") << i->first << "=\"" << i->second << "\"";
  }
  return s;
}

std::istream & operator>> (std::istream & s, Options & o){

  std::string str1, str, aname, aval;
  while (!s.eof()){
    getline(s, str1);
    std::string::size_type pos = str1.find("#");
    str+= "\n" + ((pos==std::string::npos)? str1: str1.substr(0, pos));
  }

  rule<> escaped_symb =
    (ch_p('\\') >> ch_p('=')) |
    (ch_p('\\') >> ch_p('\\')) |
    (ch_p('\\') >> ch_p('#')) |
    (ch_p('\\') >> ch_p('\n')) ;

  rule<> symb = (anychar_p - (ch_p('=') | '\\' | '#' | '\n')) | escaped_symb;

  rule<> option = *space_p >>
    (+symb)[assign_a(aname)][assign_a(aval, "")] >>
    !('=' >> (*symb)[assign_a(aval)]) >>
    !('#' >> (anychar_p - eol_p)) >>
    (eol_p | end_p)[erase_a(o, aname)][insert_at_a(o, aname, aval)];

  parse_info<> res = parse(str.c_str(), (*option)||(*space_p));
  s.clear();
  if (!res.full){
    std::cerr << "Can't parse options string \"" << str << "\" at pos " << res.length << "\n";
    s.setstate(std::ios::failbit);
  }
  return s;
}



