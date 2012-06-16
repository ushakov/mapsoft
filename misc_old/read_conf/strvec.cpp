#include "strvec.h"
#include <ios>

std::ostream & operator<< (std::ostream & s, const StrVec & v){
  for (StrVec::const_iterator i=v.begin(); i!=v.end(); i++){
    for (std::string::const_iterator c=i->begin(); c!=i->end(); c++){
      if (*c=='\n') s << '\\';
      if (*c=='\\') s << '\\';
      s << *c;
    }
    s << '\n';
  }
  return s;
}
std::istream & operator>> (std::istream & s, StrVec & v){
  v.clear();
  std::string str;
  bool qchar=false;
  char c;
  while (!(s>>c).eof()){
    if (qchar){
      if ((c!='\\') && (c!='\n')) {
        std::cerr << "StrVec operator>> warning:\n"
                  <<"   unknown escape sequence \\" << c << "\n";
      }
      str.push_back(c);
      qchar = false;
    }
    else {
      if (c == '\\') qchar = true;
      else if (c == '\n') { v.push_back(str); str.clear(); }
      else str.push_back(c);
    }
  }
  if (qchar) {
    std::cerr << "StrVec operator>> warning:\n"
              <<"   unqouted \\ at the end of input\n";
  }

  if (str.size()!=0) v.push_back(str);
  s.clear();
  return s;
}

