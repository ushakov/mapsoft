#include <cassert>

#include "cairo_wrapper.h"

void CairoExtra::save_png(const char *fname){
  Cairo::Context::get_target()->write_to_png(fname);
}

void CairoExtra::set_color_a(const int c){
  Cairo::Context::set_source_rgba(
      ((c&0xFF0000)>>16)/256.0,
      ((c&0xFF00)>>8)/256.0,
       (c&0xFF)/256.0,
      ((c&0xFF000000)>>24)/256.0
  );
}
void CairoExtra::set_color(const int c){
  Cairo::Context::set_source_rgb(
      ((c&0xFF0000)>>16)/256.0,
      ((c&0xFF00)>>8)/256.0,
       (c&0xFF)/256.0
  );
}
void CairoExtra::move_to(const dPoint & p){
  Cairo::Context::move_to(p.x, p.y);
}
void CairoExtra::line_to(const dPoint & p){
  Cairo::Context::line_to(p.x, p.y);
}
void CairoExtra::rel_move_to(const dPoint & p){
  Cairo::Context::rel_move_to(p.x, p.y);
}
void CairoExtra::rel_line_to(const dPoint & p){
  Cairo::Context::rel_line_to(p.x, p.y);
}
void CairoExtra::curve_to(const dPoint & p1, const dPoint & p2, const dPoint & p3){
  Cairo::Context::curve_to(p1.x, p1.y, p2.x, p2.y, p3.x, p3.y);
}
void CairoExtra::rel_curve_to(const dPoint & p1, const dPoint & p2, const dPoint & p3){
  Cairo::Context::rel_curve_to(p1.x, p1.y, p2.x, p2.y, p3.x, p3.y);
}
void CairoExtra::rectangle(const dRect &r){
  Cairo::Context::rectangle(r.x, r.y, r.w, r.h);
}
void CairoExtra::circle(const dPoint &p, const double r){
  Cairo::Context::begin_new_sub_path();
  Cairo::Context::arc(p.x, p.y, r, 0, 2*M_PI);
}
dPoint CairoExtra::get_current_point(){
  dPoint ret;
  Cairo::Context::get_current_point(ret.x, ret.y);
  return ret;
}
dRect CairoExtra::get_text_extents(const std::string & utf8){
  dPoint p;
  Cairo::Context::get_current_point(p.x, p.y);
  Cairo::TextExtents extents;
  Cairo::Context::get_text_extents (utf8, extents);
  return dRect(p.x+extents.x_bearing, p.y+extents.y_bearing,
               extents.width, extents.height);
}

void
CairoWrapper::reset_surface(){
  image=Image<int>();
  Cairo::RefPtr<CairoExtra>::operator=(Cairo::RefPtr<CairoExtra>());
}

void
CairoWrapper::reset_surface(int w, int h){
  image=Image<int>();
  // check if surface raw data compatable with iImage
  assert(Cairo::ImageSurface::format_stride_for_width(format, w) == w*4);

  // create surface
  Cairo::RefPtr<Cairo::ImageSurface> surface =
    Cairo::ImageSurface::create(format, w, h);

  Cairo::RefPtr<CairoExtra>::operator=
    (cast_static(Cairo::Context::create(surface)));
}

void
CairoWrapper::reset_surface(const iImage & img){
  image=img; // increase refcount of image
  // check if surface raw data compatable with iImage
  assert(Cairo::ImageSurface::format_stride_for_width(format, img.w) == img.w*4);

  // create surface
  Cairo::RefPtr<Cairo::ImageSurface> surface =
    Cairo::ImageSurface::create((unsigned char*)img.data,
      format, img.w, img.h, img.w*4);

  Cairo::RefPtr<CairoExtra>::operator=
    (cast_static(Cairo::Context::create(surface)));
}

CairoWrapper::CairoWrapper(){
}

CairoWrapper::CairoWrapper(int w, int h){
  reset_surface(w,h);
}

CairoWrapper::CairoWrapper(const iImage & img){
  reset_surface(img);
}

const Cairo::Format CairoWrapper::format = Cairo::FORMAT_ARGB32;
