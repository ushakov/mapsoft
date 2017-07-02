#include <cassert>

#include "cairo_wrapper.h"
#include "2d/line_utils.h"
#include "err/err.h"

void
CairoExtra::mkpath_smline(const dMultiLine & o, bool close, double curve_l){
  for (dMultiLine::const_iterator l=o.begin(); l!=o.end(); l++)
    mkpath_smline(*l, close, curve_l);
}

void
CairoExtra::mkpath_smline(const dLine & o, bool close, double curve_l){
  if (o.size()<1) return;
  dPoint old;
  bool first = true;
  for (dLine::const_iterator p=o.begin(); p!=o.end(); p++){
    if (p==o.begin()){
      move_to(*p);
      old=*p;
      continue;
    }
    if (curve_l==0){
      line_to(*p);
    }
    else {
      dPoint p1,p2;
      if (pdist(*p - old) > 2*curve_l){
        p1 = old + pnorm(*p - old)*curve_l;
        p2 = *p - pnorm(*p - old)*curve_l;
      }
      else {
        p1=p2=(*p+old)/2.0;
      }
      if (!first){
        curve_to(old, old, p1);
      }
      else {
        first=false;
      }
      line_to(p2);
    }
    old=*p;
  }
  if (curve_l!=0)  line_to(old);
  if (close) close_path();
}

void
CairoExtra::set_fig_font(int font, double fs, double dpi){
  std::string       face;
  Cairo::FontSlant  slant;
  Cairo::FontWeight weight;
  switch(font){
    case 0:
      face="times";
      slant=Cairo::FONT_SLANT_NORMAL;
      weight=Cairo::FONT_WEIGHT_NORMAL;
      break;
    case 1:
      face="times";
      slant=Cairo::FONT_SLANT_ITALIC;
      weight=Cairo::FONT_WEIGHT_NORMAL;
      break;
    case 2:
      face="times";
      slant=Cairo::FONT_SLANT_NORMAL;
      weight=Cairo::FONT_WEIGHT_BOLD;
      break;
    case 3:
      face="times";
      slant=Cairo::FONT_SLANT_ITALIC;
      weight=Cairo::FONT_WEIGHT_BOLD;
      break;
    case 16:
      face="sans";
      slant=Cairo::FONT_SLANT_NORMAL;
      weight=Cairo::FONT_WEIGHT_NORMAL;
      break;
    case 17:
      face="sans";
      slant=Cairo::FONT_SLANT_OBLIQUE;
      weight=Cairo::FONT_WEIGHT_NORMAL;
      break;
    case 18:
      face="sans";
      slant=Cairo::FONT_SLANT_NORMAL;
      weight=Cairo::FONT_WEIGHT_BOLD;
      break;
    case 19:
      face="sans";
      slant=Cairo::FONT_SLANT_OBLIQUE;
      weight=Cairo::FONT_WEIGHT_BOLD;
      break;
    default:
      std::cerr << "warning: unsupported fig font: " << font << "\n";
      face="sans";
      slant=Cairo::FONT_SLANT_NORMAL;
      weight=Cairo::FONT_WEIGHT_NORMAL;
  }
  if (face=="times") fs/=0.85;
  Cairo::Context::set_font_size(fs*dpi/89.0);
  Cairo::Context::set_font_face(
    Cairo::ToyFontFace::create(face, slant, weight));
}

void
CairoExtra::render_text(const char *text, dPoint pos, double ang,
       int color, int fig_font, double font_size, double dpi, int hdir, int vdir){
  Cairo::Context::save();
  set_color(color);
  set_fig_font(fig_font, font_size, dpi);

  dRect ext = get_text_extents(text);
  move_to(pos);
  Cairo::Context::rotate(ang);
  if (hdir == 1) Cairo::Context::rel_move_to(-ext.w/2, 0.0);
  if (hdir == 2) Cairo::Context::rel_move_to(-ext.w, 0.0);
  if (vdir == 1) Cairo::Context::rel_move_to(0.0, ext.h/2);
  if (vdir == 2) Cairo::Context::rel_move_to(0.0, ext.h);

  Cairo::Context::reset_clip();

  Cairo::Context::show_text(text);
  Cairo::Context::restore();
}

void
CairoExtra::render_border(const iRect & range, const dLine & brd, const int bgcolor){
  // make border path
  mkpath(brd);
  if (bgcolor!=0){
    // draw border
    set_source_rgb(0,0,0);
    set_line_width(2);
    stroke_preserve();
  }

  // erase everything outside border
  mkpath(rect2line(rect_pump(range,1)));
  set_fill_rule(Cairo::FILL_RULE_EVEN_ODD);
  if (bgcolor==0)  set_operator(Cairo::OPERATOR_CLEAR);
  else  set_color(bgcolor);
  fill();
}

Cairo::RefPtr<Cairo::SurfacePattern>
CairoExtra::img2patt(const iImage & I, double sc){
  try{
    const Cairo::Format format = Cairo::FORMAT_ARGB32;
    assert(Cairo::ImageSurface::format_stride_for_width(format, I.w) == I.w*4);
    Cairo::RefPtr<Cairo::ImageSurface> patt_surf =
      Cairo::ImageSurface::create((unsigned char*)I.data, format, I.w, I.h, I.w*4);
    Cairo::RefPtr<Cairo::SurfacePattern> patt =
      Cairo::SurfacePattern::create(patt_surf);
    Cairo::Matrix M=Cairo::identity_matrix();
    M.translate(patt_surf->get_width()/2.0, patt_surf->get_height()/2.0);
    M.scale(sc,sc);
    patt->set_matrix(M);
    return patt;
  }
  catch (Cairo::logic_error err){
    throw Err() << err.what();
  }
}

/* Cairo Wrapper functions */

void
CairoWrapper::reset_surface(){
  image=iImage();
  surface=Cairo::RefPtr<Cairo::ImageSurface>();
  Cairo::RefPtr<CairoExtra>::operator=(Cairo::RefPtr<CairoExtra>());
}

void
CairoWrapper::reset_surface(int w, int h){
  image=iImage();
  // check if surface raw data compatable with iImage
  assert(Cairo::ImageSurface::format_stride_for_width(format, w) == w*4);

  // create surface
  surface = Cairo::ImageSurface::create(format, w, h);

  Cairo::RefPtr<CairoExtra>::operator=
    (cast_static(Cairo::Context::create(surface)));
}

void
CairoWrapper::reset_surface(const iImage & img){
  image=img; // increase refcount of image
  // check if surface raw data compatable with iImage
  assert(Cairo::ImageSurface::format_stride_for_width(format, img.w) == img.w*4);

  // create surface
  surface = Cairo::ImageSurface::create((unsigned char*)img.data,
      format, img.w, img.h, img.w*4);

  Cairo::RefPtr<CairoExtra>::operator=
    (cast_static(Cairo::Context::create(surface)));
}

const Cairo::Format CairoWrapper::format = Cairo::FORMAT_ARGB32;

