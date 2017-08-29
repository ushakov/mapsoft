#ifndef ERR_H
#define ERR_H

#include <iostream>
#include <sstream>
#include <string>

/***********************************************************/
// Err -- a simple class for exceptions.

class Err {
  std::ostringstream s;    // stream for error messages
  static std::string domain; // global error domain
  int code;

  public:
    Err(int c = -1): code(c) {}
    Err(const Err & o) { code=o.code; s << o.s.str(); }

    // operator<< for error messages
    template <typename T>
      Err & operator<<(const T & o){ s << o; return *this; }

    // set/get global domain (static functions)
    static void set_domain(const std::string & d = "") {domain = d;}
    static std::string & get_domain() { return domain; }

    // get error code
    int get_code() const {return code;}

    // return error message:
    std::string get_message() const { return s.str(); }

    // return formatted error message with domain included:
    // <domain> error: <message>
    std::string get_error() const {
      return (domain.size()? domain + " " : std::string())
         + "error: " + s.str() + "\n";
    }

};

#endif
