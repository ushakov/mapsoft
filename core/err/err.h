#ifndef ERR_H
#define ERR_H

///\addtogroup libmapsoft
///@{
///\defgroup Err
///Simple class for exceptions
///@{

#include <iostream>
#include <sstream>
#include <string>

/***********************************************************/
/** Err -- a simple class for exceptions.
Example:
```
// set global error domain (if needed)
Err:set_domain("Myprog");

try {

  // throw an error with any text:
  throw Err() << "pipe " << n << "is blocked!";

  // some error code can be added (instead of default -1):
  throw Err(-2) << "some other error";
}

// catch an error:
catch (Err E){
  cerr << E.get_error();
}
```
*/

class Err {
  std::ostringstream s;    // stream for error messages
  static std::string domain; // global error domain
  int code;

  public:
    Err(int c = -1): code(c) {}
    Err(const Err & o) { code=o.code; s << o.s.str(); }

    /// Operator << for error messages.
    template <typename T>
      Err & operator<<(const T & o){ s << o; return *this; }

    /// Set/get global domain (static functions).
    static void set_domain(const std::string & d = "") {domain = d;}
    static std::string & get_domain() { return domain; }

    /// Get error code.
    int get_code() const {return code;}

    /// Return error message.
    std::string get_message() const { return s.str(); }

    /// Return formatted error message with domain included:
    /// <domain> error: <message>.
    std::string get_error() const {
      return (domain.size()? domain + " " : std::string())
         + "error: " + s.str() + "\n";
    }

};

#endif
