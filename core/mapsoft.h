#ifndef MAPSOFT_H
#define MAPSOFT_H

// error handling
#include <err/err.h>

// 2D library
#include <2d/cache.h>
#include <2d/conv_aff.h>
#include <2d/conv.h>
#include <2d/conv_multi.h>
#include <2d/conv_triv.h>
#include <2d/image.h>
#include <2d/image_source.h>
#include <2d/line_dist.h>
#include <2d/line.h>
#include <2d/line_polycrop.h>
#include <2d/line_rectcrop.h>
#include <2d/line_tests.h>
#include <2d/line_utils.h>
#include <2d/point_cross.h>
#include <2d/point.h>
#include <2d/point_int.h>
#include <2d/rainbow.h>
#include <2d/rect.h>
#include <2d/sizecache.h>

#include <fig/fig_data.h>
#include <fig/fig.h>
#include <fig/fig_io.h>
#include <fig/fig_mask.h>
#include <fig/fig_utils.h>

#include <geo/geo_convs.h>
#include <geo/geo_data.h>
#include <geo/geo_nom.h>
#include <geo/geo_types.h>
#include <geo/g_map.h>
#include <geo/g_trk.h>
#include <geo/g_wpt.h>

#include <geo_io/geofig.h>
#include <geo_io/geo_refs.h>
#include <geo_io/io_gps.h>
#include <geo_io/io_gpx.h>
#include <geo_io/io_gu.h>
#include <geo_io/io.h>
#include <geo_io/io_js.h>
#include <geo_io/io_kml.h>
#include <geo_io/io_oe.h>
#include <geo_io/io_xml.h>
#include <geo_io/io_zip.h>

#include <gred/action.h>
#include <gred/action_manager.h>
#include <gred/dthread_viewer.h>
#include <gred/gobj.h>
#include <gred/gred.h>
#include <gred/rubber.h>
#include <gred/simple_viewer.h>
#include <gred/viewer.h>

#include <img_io/draw_trk.h>
#include <img_io/draw_wpt.h>
#include <img_io/gobj_comp.h>
#include <img_io/gobj_geo.h>
#include <img_io/gobj_grid_pulk.h>
#include <img_io/gobj_map.h>
#include <img_io/gobj_pano.h>
#include <img_io/gobj_srtm.h>
#include <img_io/gobj_srtmv.h>
#include <img_io/gobj_trk.h>
#include <img_io/gobj_vmap.h>
#include <img_io/gobj_wpt.h>
#include <img_io/io.h>
#include <img_io/o_img.h>
#include <img_io/o_tiles.h>

#include <loaders/image_cache.h>
#include <loaders/image_gif.h>
#include <loaders/image_jpeg.h>
#include <loaders/image_png.h>
#include <loaders/image_r.h>
#include <loaders/image_tiff.h>

#include <mp/mp_data.h>
#include <mp/mp.h>
#include <mp/mp_io.h>
#include <mp/mp_mask.h>

#include <ocad/ocad_colors.h>
#include <ocad/ocad_file.h>
#include <ocad/ocad_fname.h>
#include <ocad/ocad_geo.h>
#include <ocad/ocad_header.h>
#include <ocad/ocad_index.h>
#include <ocad/ocad_object8.h>
#include <ocad/ocad_object9.h>
#include <ocad/ocad_object.h>
#include <ocad/ocad_shead.h>
#include <ocad/ocad_string.h>
#include <ocad/ocad_symbol8.h>
#include <ocad/ocad_symbol9.h>
#include <ocad/ocad_symbol.h>
#include <ocad/ocad_types.h>

#include <options/m_color.h>
#include <options/m_getopt.h>
#include <options/m_time.h>
#include <options/options.h>

#include <srtm/srtm3.h>
#include <srtm/tracers.h>

#include <utils/cairo_wrapper.h>
#include <utils/err.h>
#include <utils/generic_accessor.h>
#include <utils/iconv_utils.h>
#include <utils/image_gdk.h>
#include <utils/log.h>
#include <utils/pnm_shifter.h>
#include <utils/spirit_utils.h>

#include <vmap/vmap.h>
#include <vmap/zn.h>
#include <vmap/zn_lists.h>

#endif