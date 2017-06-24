#ifndef GOBJ_FIG_H
#define GOBJ_FIG_H

#include "libfig/fig.h"

#include "gred/gobj.h"
#include "lib2d/line.h"
#include "lib2d/line_rectcrop.h"
#include "2d/cache.h"

#include "gobj_line.h"

#include <iostream>
#include <cairomm/cairommconfig.h>
#include <cairomm/context.h>
#include <cairomm/surface.h>
#include <cstring>

#define FIG_CACHE_SIZE1 1000
#define FIG_CACHE_SIZE2 1000
#define FIG_TH_FACTOR 8
/*****************/

fig::fig_world
crop_and_scale_fig(const fig::fig_world & F,
  const iRect & cutter, const double sc){

  fig::fig_world ret(F); // copy the whole fig!

  fig::fig_world::iterator o=ret.begin();
  while (o!=ret.end()){

    for (fig::fig_object::iterator p=o->begin(); p!=o->end(); p++){
      (*p)=dPoint(*p)*sc*72.0/1200.0;
    }
    rect_crop(rect_pump(cutter, int(o->thickness*sc)),
              *o, o->is_closed());
    if (o->size()==0){
      o=ret.erase(o); continue;
    }
    if (o->thickness){
      o->thickness*=FIG_TH_FACTOR*sc;
      if (o->thickness==0) o->thickness=1;
    }
    o++;
  }
  return ret;
}
/*


  fig::fig_world ret;
  fig::fig_world::const_iterator o;
  for (o=F.begin(); o!=F.end(); o++){
    iMultiLine m1, m2;
    m1.push_back(*o);
    m2=crop_and_scale_multiline(m1,
      rect_pump(cutter, int(o->thickness*sc)), sc*72.0/1200.0);

    for (iMultiLine::iterator i=m2.begin(); i!=m2.end(); i++){
      fig::fig_object o1(; = o->copy_empty();
      o1.set_points(*i);
      o1.thickness*=FIG_TH_FACTOR;
      ret.push_back(o1);
    }
  }
  return ret;
}
*/

class CairoWrapper /*: public Cairo::Context*/{
public:
/*
  static static Cairo::RefPtr<CairoWrapper> create(){
    Cairo::Format fmt = Cairo::FORMAT_ARGB32;

    // check stride value to be compatable with iImage
    int stride = Cairo::ImageSurface::format_stride_for_width(fmt, img.w);
    assert(stride = 4*img.w);

    // create surface and context
    Cairo::RefPtr<Cairo::ImageSurface> surface =
      Cairo::ImageSurface::create((unsigned char *)img.data, fmt, img.w, img.h, stride);
    Cairo::RefPtr<Cairo::CairoWrapper> cr = Cairo::Context::create(surface);
    return cr;
  }
*/
  Cairo::RefPtr<Cairo::ImageSurface> surface;
  Cairo::RefPtr<Cairo::Context>      context;

  CairoWrapper(const iImage &img){

    // check stride value to be compatable with iImage
    Cairo::Format fmt = Cairo::FORMAT_ARGB32;
    int stride = Cairo::ImageSurface::format_stride_for_width(fmt, img.w);
    assert(stride = 4*img.w);

    // create surface and context
    surface = Cairo::ImageSurface::create((unsigned char *)img.data, fmt, img.w, img.h, stride);
    context = Cairo::Context::create(surface);
  }
  void set_rgb(int c){
    context->set_source_rgb(
     ((c & 0xff0000)>>16)/256.0,
     ((c & 0xff00)>>8)/256.0,
     (c & 0xff)/256.0);
  }
  void move_to(const dPoint & p){ context->move_to(p.x, p.y);}
  void line_to(const dPoint & p){ context->line_to(p.x, p.y);}
  void curve_to(const dPoint & p1, const dPoint & p2, const dPoint & p3){
    context->curve_to(p1.x, p1.y, p2.x, p2.y, p3.x, p3.y);
  }
};


LINE_CAP_BUTT
LINE_CAP_ROUND
LINE_CAP_SQUARE

LINE_JOIN_MITER
LINE_JOIN_ROUND
LINE_JOIN_BEVEL

/// draw fig object
int
draw(const fig::fig_object & o, CairoWrapper & cr, const iPoint &origin){
  if (o.size()<1) return GObj::FILL_NONE;

  o.line_style
  o.thickness
  o.pen_color
  o.fill_color

  set_line_cap(o.join_style
  set_line_join(o.cap_style
  set_line_width(o.thickness);

  cr.set_rgb(o.pen_color);
  cr.context->set_line_width(o.thickness*1.0/FIG_TH_FACTOR);

  switch (o.type){
    case 1: // ellipse
      switch (o.subtype){
        case 1: // ellipse defined by radii
          // ...
          break;
        case 2: // ellipse defined by diameters
          // ...
          break;
        case 4: // circle defined by radius
          // ...
          break;
        case 5: // circle defined by diameter
          // ...
          break;
      }
      break;
    case 2: // polyline
      // ...
      break;
    case 3: // spline
      // ...
      break;
    case 4: // text
      // ...
      break;
    case 5: // arc
      // ...
      break;
  }

  cr.move_to(o[0]-origin);
  for (int i=1; i<o.size(); i++) cr.line_to(o[i]-origin);
  if (o.is_closed()) cr.context->close_path();

  cr.context->stroke();
  return GObj::FILL_PART;
}

// fig objects must be sorted by depth!
int
draw_fig(const fig::fig_world & F, iImage &img, const iPoint &origin){

  CairoWrapper cr(img);

  // draw
  fig::fig_world::const_iterator o;
  for (o=F.begin(); o!=F.end(); o++) draw(*o, cr, origin);
  cr.surface->finish();
  return GObj::FILL_PART;
}


//iMultiLine

/******************************/

class GObjFIG: public GObj{
  const fig::fig_world & F;
  iRect myrange;
  double sc;

  static Cache<GObjFIG *, Cache<iRect, fig::fig_world> > cache;

public:
  GObjFIG(const fig::fig_world & F_): F(F_),
       sc(1.0), myrange(F.range()){
  }

  void set_scale(const double k){
    sc=k;
    myrange = dRect(F.range())*k;
    refresh();
  }
  void refresh(){
    if (cache.contains(this)) cache.get(this).clear();
  }


  int draw(iImage &img, const iPoint &origin){
    iRect r(origin.x, origin.y, img.w, img.h);

/// debugging: show tile bounds
//for (int a = 0; a< img.w; a++) img.set(a,0, 0xFFFF0000);
//for (int a = 0; a< img.h; a++) img.set(0,a, 0xFFFF0000);

    if (!cache.contains(this))
      cache.add(this, Cache<iRect, fig::fig_world>(FIG_CACHE_SIZE2));

    Cache<iRect, fig::fig_world> & cache2 = cache.get(this);

    if (!cache2.contains(r))
      cache2.add(r,  crop_and_scale_fig(F, r, sc));

    fig::fig_world & f = cache2.get(r);
    return draw_fig(f, img, origin);
  }

  iRect range() const{
    return myrange;
  }

};

Cache<GObjFIG *, Cache<iRect, fig::fig_world> > GObjFIG::cache(FIG_CACHE_SIZE1);

#endif
