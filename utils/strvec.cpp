#include "strvec.h"

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
  bool append=false;
  while (!s.eof()){
    getline(s, str);
    if ((v.size()>0) &&
        (v.rbegin()->size()>0) &&
        (*(v.rbegin()->rbegin()) == '\\')){
      *(v.rbegin()->rbegin()) = '\n';
      *(v.rbegin()) += str;
    }
    else v.push_back(str);
  }
  return s;
}

