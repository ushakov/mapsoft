#ifndef O_IMG_H
#define O_IMG_H

#include "geo/geo_data.h"
#include "options/options.h"

namespace img {

/*
### MANPAGE TEXT

=head1 OPTIONS FOR RENDERING IMAGES

B<<  --htm <file> >> -- write html map into file

B<<  -f, --fig <file> >> -- write fig-file

B<<  -m, --map <file> >> -- write map-file

B<<  --draw_borders >> -- draw map borders

B<<  --trk_draw_mode <mode> >> -- draw mode: normal, speed, height 

B<<  --trk_draw_dots >> -- draw dots on tracks

B<<  --trk_draw_arrows >> -- draw arrows on tracks 

B<<  --trk_draw_v1 <v1> >> -- low value for speed draw mode

B<<  --trk_draw_v2 <v2> >> -- high value for speed draw mode

B<<  --trk_draw_h1 <h1> >> -- low value for height draw mode

B<<  --trk_draw_h2 <h2> >> -- high value for height draw mode

B<<  --max_image <W,H> >> -- don't write images larger then this, default 1000,1000

B<<  --data_marg <arg> >> -- margins around data (works only if no geometry set), pixels

B<< --jpeg_quality <arg> >> -- set jpeg quality

### OPTIONS

  {"htm",            1,  0, OPT, "write html map into file"},
  {"fig",            1,'f', OPT, "write fig-file"},
  {"map",            1,'m', OPT, "write map-file"},
  {"draw_borders",   0,  0, OPT, "draw map borders"},
  {"trk_draw_mode"  ,1,  0, OPT, "draw mode: normal, speed, height"},
  {"trk_draw_dots",  0,  0, OPT, "draw dots on tracks"},
  {"trk_draw_arrows",0,  0, OPT, "draw arrows on tracks"},
  {"trk_draw_v1"    ,1,  0, OPT, "low value for speed draw mode"},
  {"trk_draw_v2"    ,1,  0, OPT, "high value for speed draw mode"},
  {"trk_draw_h1"    ,1,  0, OPT, "low value for height draw mode"},
  {"trk_draw_h2"    ,1,  0, OPT, "high value for height draw mode"},

  {"srtm_mode" ,     1,  0, OPT, "srtm mode: normal, slopes"},
  {"srtm_cnt_step",  1,  0, OPT, "contour step, m"},
  {"srtm_hmin",      1,  0, OPT, "min height (blue color) for normal mode"},
  {"srtm_hmax",      1,  0, OPT, "max height (magenta color) for normal mode"},
  {"srtm_smin",      1,  0, OPT, "min slope for slopes mode (30)"},
  {"srtm_smax",      1,  0, OPT, "max slope for slopes mode (55)"},

  {"max_image",      1,  0, OPT, "don't write images larger then this, \"x,y\", default 1000,1000"},
  {"data_marg",      1,  0, OPT, "margins around data (works only if no geometry set), pixels"},
  {"jpeg_quality",   1,  0, OPT, "set jpeg quality"},

### END
*/
    bool write_file (const char* filename, const geo_data & world, vmap::world & vm, Options opt);

} //namespace
#endif