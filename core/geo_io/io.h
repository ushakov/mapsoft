#ifndef MAPSOFT_IO_H
#define MAPSOFT_IO_H

// function for I/O

#include <string>
#include "geo/geo_data.h"
#include "options/options.h"

#include "io_gps.h"
#include "io_gu.h"
#include "io_xml.h"
#include "io_kml.h"
#include "io_gpx.h"
#include "io_oe.h"
#include "io_zip.h"
#include "o_img.h"
#include "o_tiles.h"

namespace io {
  // read data from file
  bool in(const std::string & in_name, geo_data & world, const Options & opt);

  // write data to file
  void out(const std::string & out_name, const geo_data & world, const Options & opt);

  // check file extension
  bool testext(const std::string & nstr, char *ext);

  // audodetect garmin_gps device
  std::string gps_detect();

  // -1 -- can't access file; 0 - regular, 1 - character device
  int check_file(const std::string & name);

  // filters

  // добавить границу в соответствии с названием номенклатурного листа
  void map_nom_brd(geo_data & world);

  // уменьшить число точек трека
  void generalize(g_track * line, double e, int np);

  // erase some data according to "skip" option
  void skip(geo_data & world, const std::string & sk);

  // filter geodata according to options:
  // --shift_maps x,y  -- shift map references
  // --rescale_maps k  -- rescale map references
  // --map_nom_brd     -- set map borders according to map name
  // --skip wmtao      -- skip data (w - waypoints, m - maps, t - tracks,
  //                      a - active log, o - save tracks)
  // --gen_n           -- reduce track points to n
  // --gen_e           -- reduce track points up to accuracy e [meters]
  //   (when gen_n and gen_e both used it means: "remove points while
  //   number of points > n OR accuracy < e")
  void filter(geo_data & world, const Options & opt);
}

#endif

