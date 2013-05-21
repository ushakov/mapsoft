#ifndef CAIRO_WRAPPER_H
#define CAIRO_WRAPPER_H

#include <cairomm/context.h>
#include <cairomm/surface.h>

#include <string>

#include "2d/image.h"
#include "2d/point.h"
#include "2d/line.h"
#include "2d/rect.h"

/// This class contains functions
/// we want to add to the Cairo::Context
struct CairoExtra : public Cairo::Context {

  void save_png(const char *fname){
    Cairo::Context::get_target()->write_to_png(fname); }


  // Note: color order is inversed! It is good for
  // Gtk::pixbuf and mapsoft:iImage, but you can't
  // load/save png using Cairo functions

  // Another problem is prescaled colors in semi-transparent
  // images: cairo uses prescaled colors, mapsoft uses them not in
  // all places

  void set_color_a(const int c){
    Cairo::Context::set_source_rgba(
       (c&0xFF)/256.0,
      ((c&0xFF00)>>8)/256.0,
      ((c&0xFF0000)>>16)/256.0,
      ((c&0xFF000000)>>24)/256.0
    );
  }
  void set_color(const int c){
    Cairo::Context::set_source_rgb(
       (c&0xFF)/256.0,
      ((c&0xFF00)>>8)/256.0,
      ((c&0xFF0000)>>16)/256.0
    );
  }

  // move_to/line_to functions for dPoint arguments

  void move_to(const dPoint & p){
    Cairo::Context::move_to(p.x, p.y); }

  void line_to(const dPoint & p){
    Cairo::Context::line_to(p.x, p.y); }

  void rel_move_to(const dPoint & p){
    Cairo::Context::rel_move_to(p.x, p.y); }

  void rel_line_to(const dPoint & p){
    Cairo::Context::rel_line_to(p.x, p.y); }

  void curve_to(const dPoint & p1, const dPoint & p2, const dPoint & p3){
    Cairo::Context::curve_to(p1.x, p1.y, p2.x, p2.y, p3.x, p3.y); }

  void rel_curve_to(const dPoint & p1, const dPoint & p2, const dPoint & p3){
    Cairo::Context::rel_curve_to(p1.x, p1.y, p2.x, p2.y, p3.x, p3.y); }

  void rectangle(const dRect &r){
    Cairo::Context::rectangle(r.x, r.y, r.w, r.h);
  }
  void circle(const dPoint &p, const double r){
    Cairo::Context::begin_new_sub_path();
    Cairo::Context::arc(p.x, p.y, r, 0, 2*M_PI);
  }
  dPoint get_current_point(){
    dPoint ret;
    Cairo::Context::get_current_point(ret.x, ret.y);
    return ret;
  }
  dRect get_text_extents(const std::string & utf8){
    dPoint p;
    Cairo::Context::get_current_point(p.x, p.y);
    Cairo::TextExtents extents;
    Cairo::Context::get_text_extents (utf8, extents);
    return dRect(p.x+extents.x_bearing, p.y+extents.y_bearing,
                 extents.width, extents.height);
  }

  // Make paths from dLine/dMultiline argument

  void mkpath(const dLine & o, bool close=true){
    for (dLine::const_iterator p=o.begin(); p!=o.end(); p++){
      if (p==o.begin()) move_to(*p);
      else line_to(*p);
    }
    if (close) close_path();
  }
  void mkpath(const dMultiLine & o, bool close=true){
    for (dMultiLine::const_iterator l=o.begin(); l!=o.end(); l++)
      mkpath(*l, close);
  }

  void mkpath_points(const dLine & o){
    for (dLine::const_iterator p=o.begin(); p!=o.end(); p++){
      move_to(*p);
      Cairo::Context::rel_line_to(0,0);
    }
  }

  void mkpath_points(const dMultiLine & o){
    for (dMultiLine::const_iterator l=o.begin(); l!=o.end(); l++)
      mkpath_points(*l);
  }

  // make path for smoothed line
  void mkpath_smline(const dMultiLine & o, bool close=true, double curve_l=0);
  void mkpath_smline(const dLine & o, bool close=true, double curve_l=0);

  // some short functions for cap/miter setting
  void cap_round()  { set_line_cap(Cairo::LINE_CAP_ROUND);  }
  void cap_butt()   { set_line_cap(Cairo::LINE_CAP_BUTT);   }
  void cap_square() { set_line_cap(Cairo::LINE_CAP_SQUARE); }
  void join_miter() { set_line_join(Cairo::LINE_JOIN_MITER); }
  void join_round() { set_line_join(Cairo::LINE_JOIN_ROUND); }

  // short functions for dash line settings
  void set_dash(std::vector<double> d){
    Cairo::Context::set_dash(d, 0);
  }
  void set_dash(double d1, double d2){
    std::vector<double> d;
    d.push_back(d1);
    d.push_back(d2);
    Cairo::Context::set_dash(d, 0);
  }
  void set_dash(double d1, double d2, double d3, double d4){
    std::vector<double> d;
    d.push_back(d1);
    d.push_back(d2);
    d.push_back(d3);
    d.push_back(d4);
    Cairo::Context::set_dash(d, 0);
  }

};

/*** CairoWrapper - we need this to create RefPtr<CairoExtra> ***/

struct CairoWrapper: Cairo::RefPtr<CairoExtra> {
private:
  // image is used only in reset_surface(img)
  // for increasing refcounter of the original image
  iImage image;
  const static Cairo::Format format;
  Cairo::RefPtr<Cairo::ImageSurface> surface;

public:

  void reset_surface();
  void reset_surface(int w, int h);
  void reset_surface(const iImage & img);

  CairoWrapper(){}

  CairoWrapper(int w, int h){ reset_surface(w,h); }
  CairoWrapper(const iImage & img){ reset_surface(img); }

  Cairo::RefPtr<Cairo::ImageSurface> get_im_surface() { return surface; }
};
#endif
