#ifndef DRAW_WPT_H
#define DRAW_WPT_H

#include "2d/image.h"
#include "2d/conv.h"
#include "geo/g_wpt.h"
#include "options/options.h"

void
draw_wpt(iImage & image, const iPoint & origin,
         const Conv & cnv, const g_waypoint_list & wpt,
         const Options & opt);

#endif
