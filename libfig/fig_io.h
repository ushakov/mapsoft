#ifndef FIG_IO_H
#define FIG_IO_H

#include <iostream>

#include "fig_data.h"

namespace fig {
    bool read(const char* filename, fig_world & world);
    bool write(std::ostream & out, const fig_world & world);
}
#endif
