#ifndef MAPSOFT_IO_H
#define MAPSOFT_IO_H

// function for I/O

#include <string>
#include "../libgeo/geo_data.h"
#include "../utils/options.h"

#include "io_gps.h"
#include "io_gu.h"
#include "io_xml.h"
#include "io_kml.h"
#include "io_oe.h"
#include "o_img.h"


/** Reading data from file into geo_data object. */
void geo_read(const std::string & file, geo_data & world, const Options & opts=Options());

/** Applying filters to geo_data. */
void geo_filter(const geo_data & world, const Options & opts=Options());

/** Writing geo_data object to file. */
void geo_write(const std::string & file, const geo_data & world, const Options & opts=Options());

namespace io {
        // read data from file
	bool in(const std::string & name, geo_data & world, const Options & opt);

        // write data to file
        void out(const std::string & outfile, const geo_data & world, const Options & opt);

        bool testext(const std::string & nstr, char *ext);

        // erase some data according to "skip" option
	void skip(geo_data & world, const Options & opt);
}

namespace filters {
  // границы номенклатурного листа в wgs84
  Rect<double> nom_range(const std::string & key);

  // добавить границу в соответствии с названием номенклатурного листа
  void map_nom_brd(geo_data & world);
}

#endif

