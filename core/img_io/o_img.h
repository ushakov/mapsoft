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

B<<  --max_image <W,H> >> -- don't write images larger then this, default 1000,1000

B<<  --data_marg <arg> >> -- margins around data (works only if no geometry set), pixels

B<< --jpeg_quality <arg> >> -- set jpeg quality

### OPTIONS

  {"htm",           1,  0, OPT3, "write html map into file"},
  {"fig",           1,'f', OPT3, "write fig-file"},
  {"map",           1,'m', OPT3, "write map-file"},
  {"draw_borders",  0,  0, OPT3, "draw map borders"},
  {"max_image",     1,  0, OPT3, "don't write images larger then this, \"x,y\", default 1000,1000"},
  {"data_marg",     1,  0, OPT3, "margins around data (works only if no geometry set), pixels"},
  {"jpeg_quality",  1,  0, OPT3, "set jpeg quality"},

  {"ks_zoom",      0,  0, OPT4, ""},
  {"google_zoom",  0,  0, OPT4, ""},
  {"google_dir",   0,  0, OPT4, ""},
  {"ks_dir",       0,  0, OPT4, ""},
  {"download",     0,  0, OPT4, ""},

### END
*/
    bool write_file (const char* filename, const geo_data & world, Options opt);

} //namespace
#endif