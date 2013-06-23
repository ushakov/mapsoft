#ifndef DRAW_TRK_H
#define DRAW_TRK_H

#include "2d/image.h"
#include "2d/conv.h"
#include "geo/g_trk.h"
#include "options/options.h"

void
draw_trk(iImage & image, const iPoint & origin,
         const Conv & cnv, const g_track & trk,
         const Options & opt);

#endif
