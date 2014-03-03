#ifndef DRAW_WPT_H
#define DRAW_WPT_H

#include "2d/image.h"
#include "2d/conv.h"
#include "geo/g_wpt.h"
#include "options/options.h"
#include "utils/cairo_wrapper.h"

/* draw waypoints */
void draw_wpt(iImage & image, const iPoint & origin,
              const Conv & cnv, const g_waypoint_list & wpt,
              const Options & opt);

iRect wpts_range(const Conv & cnv,
                 const g_waypoint_list & wpt,
                 const Options & opt);

/***********************************************************/

/* Drawing can be splitted into two parts:
   Creating small drawing template (can be slow)
   and then drawing it.
   Also, while creating template we go through
   all wpts and can calculate the total range */

/* waypoint templates */
struct tmpl_wpt : dPoint{
  double font_size, dot_rad, text_pad;
  int color, bgcolor;
  dRect txt_rect;
  std::string name;
};
typedef std::vector<tmpl_wpt> tmpl_wpts;

/* make template and calculate total range */
tmpl_wpts make_wpts_tmpl(CairoWrapper & cr, const iPoint & origin,
                         const Conv & cnv, const g_waypoint_list & wpt,
                         const Options & opt, iRect & range);

/* plot template */
void plot_wpts_tmpl(CairoWrapper & cr, const tmpl_wpts & wpts);


#endif
