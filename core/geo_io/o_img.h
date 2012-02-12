#ifndef O_IMG_H
#define O_IMG_H

#include "geo/geo_data.h"
#include "options/options.h"

namespace img {

/* Options:
  --htm <file>    -- write html map into file
  --fig <file>    -- write fig-file
  --map <file>    -- write map-file
  --draw_borders  -- draw map borders
  --max_image <x,y>    -- don't write images larger then this, "x,y", default 1000,1000
  --data_marg <meters> -- margins around data (works only if no geometry set)

  --ks_zoom <z>
  --google_zoom <z>
  --google_dir <dir>
  --ks_dir <dir>
  --download
*/
    bool write_file (const char* filename, const geo_data & world, Options opt);

} //namespace
#endif