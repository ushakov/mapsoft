#ifndef MP_IO_H
#define MP_IO_H

#include <iostream>
#include <string>

#include "mp_data.h"
#include "../utils/options.h"


namespace mp {

  /// Common options:
  ///  * mp_codepage <cpage> -- Codepage for text. Used for writing
  ///    files and for reading files with unspecified codepage.
  ///    Default value is 1251.
  ///    Encoding = "CP" + codepage.

  /// Read mp-file specified by <filename> into <world>. If some data
  /// already exists in <world> then objects are appended and the header
  /// is overwrited...
  /// Text is converted to UTF8 encoding

  bool read(const char* filename, mp_world & world, const Options & opts = Options());

  /// Write <world> into mp-file <filename>
  bool write(std::ostream & out, const mp_world & world, const Options & opts = Options());
  bool write(const std::string & file, const mp_world & world, const Options & opts = Options());

}
#endif
