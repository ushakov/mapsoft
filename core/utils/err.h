#ifndef MAPSOFT_ERR_H
#define MAPSOFT_ERR_H

#include <string>
#include <sstream>

/*
  Class for mapsoft exceptions.
  Example:

  try {
    if (SomethingWrong) throw MapsoftErr("WRONG1") << "Something is wrong";
    if (SomethingElse) throw apsoftErr("WRONG2") << "Something else is wrong";
  }
  catch (MapsoftErr err){
    cerr << err.str() << endl;
    if (err.id() == "WRONG1") exit(1):
    else exit(2);
  }
*/

class MapsoftErr{
  const std::string i;
  std::ostringstream s;

public:
  MapsoftErr(const std::string & id = std::string());
  MapsoftErr(const MapsoftErr & o);
  std::string str() const;
  std::string id() const;

  template <typename T>
  MapsoftErr & operator<<(const T & o){
    s << o;
    return *this;
  }

};


#endif

