#ifndef CAIRO_WRAPPER_H
#define CAIRO_WRAPPER_H

#include <cairomm/cairommconfig.h>
#include <cairomm/context.h>
#include <cairomm/surface.h>

#include <string>

#include "2d/image.h"
#include "2d/point.h"
#include "2d/rect.h"

struct CairoExtra : public Cairo::Context {

  void save_png(const char *fname);
  void set_color_a(const int c);
  void set_color(const int c);
  void move_to(const dPoint & p);
  void line_to(const dPoint & p);
  void rel_move_to(const dPoint & p);
  void rel_line_to(const dPoint & p);
  void curve_to(const dPoint & p1, const dPoint & p2, const dPoint & p3);
  void rel_curve_to(const dPoint & p1, const dPoint & p2, const dPoint & p3);
  void rectangle(const dRect &r);
  void circle(const dPoint &p, const double r);
  dPoint get_current_point();
  dRect  get_text_extents(const std::string & utf8);
};

struct CairoWrapper: Cairo::RefPtr<CairoExtra> {
  const static Cairo::Format format;
  CairoWrapper(int w, int h);
  CairoWrapper(const iImage & img);
};
#endif
