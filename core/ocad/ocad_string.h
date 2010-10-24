#ifndef OCAD_STRING_H
#define OCAD_STRING_H

#include <cstdio>
#include <string>
#include "ocad_types.h"
#include <boost/lexical_cast.hpp>

namespace ocad{

/// This is OCAD8/9 string. Such strings contain many
/// various information (setup, colors, geo-reference etc.)

struct ocad_string{


  ocad_long type;
  ocad_long obj;
  std::string data;

  /// Empty constructor.
  ocad_string();

  /// Get string value for field f.
  std::string get_str(const char f = 0) const;

  /// Get value for field f casted to type T.
  template <typename T>
  T get(const char f) const{
    return boost::lexical_cast<T>(get_str(f));
  }

  /// Dump string to stdout.
  void dump(int verb) const;


  /// Index type and read/write functions are used in ocad_index
  /// template for reading/writing OCAD files.

  /// Low-level index entry type.
  struct index{
    ocad_long pos;  // file position of string
    ocad_long len;  // length reserved for the string
    ocad_long type; // string typ number, if < 0 then deleted
    ocad_long obj;  // number of the object from 1 (or 0)
    index(): pos(0),len(0),type(0),obj(0){ }
  };

  /// Read string from file.
  void read(FILE * F, index idx, int v);

  /// Write string to file.
  index write(FILE * F, int v) const;
};

} // namespace
#endif
