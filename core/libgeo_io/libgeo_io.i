%{
#include "libgeo_io/io.h"
%}

/** Read data from file into geo_data object. */
void geo_read(const std::string & file, geo_data & world, const Options & opts=Options());
/** Write geo_data object to file. */
void geo_write(const std::string & file, const geo_data & world, const Options & opts=Options());

