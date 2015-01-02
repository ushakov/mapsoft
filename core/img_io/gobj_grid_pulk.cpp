#include "gobj_grid_pulk.h"
#include <2d/line_utils.h>
#include <sstream>

int
GObjGridPulk::draw(iImage & image, const iPoint & origin){

  /* find wgs coordinate range and 6 degree zones */
  const g_map m = *get_ref();
  dPoint dorg(origin);
  dRect rng = image.range() + dorg;
  dRect rng_wgs = cnv.bb_frw(rng); // screen -> wgs
  int lon0a = convs::lon2lon0(rng_wgs.TLC().x);
  int lon0b = convs::lon2lon0(rng_wgs.BRC().x);

  /* for all zones */
  for (int lon0=lon0a; lon0<=lon0b; lon0+=6){

    /* build  screen -> pulkovo conversion or get it from the cache */
    if (!convs.contains(lon0)){
      Options o;
      o.put("lon0", lon0);
      convs.add(lon0, convs::map2pt(m, Datum("pulk"), Proj("tmerc"), o));
    }
    convs::map2pt cnv1(convs.get(lon0));

    dRect rng_pulk = cnv1.bb_frw(rng); // screen -> pulkovo
    if (isinf(rng_pulk.w) || isinf(rng_pulk.w)) continue;
    double step = 1000;
    while (rng_pulk.w> step*10 || rng_pulk.h> step*10) step*=10.0;
    while (rng_pulk.w< step/10 || rng_pulk.h< step/10) step/=10.0;

    dPoint tlc = rng_pulk.TLC(), brc= rng_pulk.BRC();
    double xmin = floor(tlc.x/step)*step;
    double xmax = ceil(brc.x/step)*step;
    double ymin = floor(tlc.y/step)*step;
    double ymax = ceil(brc.y/step)*step;

    CairoWrapper cr(image);
    cr->set_source_rgba(1,0,0,1);
    cr->set_line_width(2);
    if (lon0a!=lon0b){ /* clip the zone */
      dRect rng_clip(lon0-3.0, rng_wgs.y, 6.0, rng_wgs.h);
      rng_wgs.y-=rng_wgs.h*0.2;
      rng_wgs.h+=rng_wgs.h*0.4;
      cr->mkpath(cnv.line_bck(rect2line(rng_clip,1)) - dorg);
      cr->clip_preserve();
      cr->stroke();
    }

    for (double x=xmin; x<=xmax; x+=step){
      dPoint p1(x, tlc.y), p2(x, brc.y);
      cnv1.bck(p1), cnv1.bck(p2);
      cr->move_to(p1-dorg);
      cr->line_to(p2-dorg);
    }
    for (double y=ymin; y<=ymax; y+=step){
      dPoint p1(tlc.x, y), p2(brc.x, y);
      cnv1.bck(p1), cnv1.bck(p2);
      cr->move_to(p1-dorg);
      cr->line_to(p2-dorg);
    }
    cr->stroke();

    /* step for labels */
    step*=2;
    xmin = floor(tlc.x/step)*step;
    xmax = ceil(brc.x/step)*step;
    ymin = floor(tlc.y/step)*step;
    ymax = ceil(brc.y/step)*step;

    /* labels */
    cr->set_fig_font(18, 15, 100);
    for (double x=xmin; x<=xmax; x+=step){
      for (double y=ymin; y<=ymax; y+=step){
        stringstream sx, sy;
        sx << int(x/1000), sy << int(y/1000);
        dPoint p1(x,y); cnv1.bck(p1);
        cr->move_to(p1-dorg + dPoint(5, -5));
        cr->show_text(sy.str());
        cr->move_to(p1-dorg + dPoint(20, -25));
        cr->save();
        cr->rotate(-M_PI/2.0);
        cr->show_text(sx.str());
        cr->restore();
      }
    }

  }

  return GOBJ_FILL_PART;
}


