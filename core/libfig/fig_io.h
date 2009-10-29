#ifndef FIG_IO_H
#define FIG_IO_H

#include <iostream>

#include "fig_data.h"
#include "../utils/options.h"

namespace fig {

    ///   Common Options:
    /// * fig_enc=<enc> -- encoding for reading and writing files.
    ///   Default value = KOI8-R

    /// Read fig-file specified by <filename> into <world>. If some data
    /// already exists in <world> then objects are appended and the header
    /// is overwrited...
    /// Non-standart colors are converted to "long" values (> 0x1000000)
    /// Text is converted to UTF8 encoding.
    bool read(const char* filename, fig_world & world, const Options & opts = Options());

    /// Write <world> into fig-file <filename>
    /// Some tests are performed here:
    /// - comments must be not more then 100 lines, 1022 chars each
    bool write(std::ostream & out, const fig_world & world, const Options & opts = Options());
    bool write(const std::string & file, const fig_world & world, const Options & opts = Options());

}
#endif
