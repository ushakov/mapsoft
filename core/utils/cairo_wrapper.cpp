#include <cassert>

#include "cairo_wrapper.h"

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


/* Cairo Wrapper functions */

void
CairoWrapper::reset_surface(){
  image=Image<int>();
  surface=Cairo::RefPtr<Cairo::ImageSurface>();
  Cairo::RefPtr<CairoExtra>::operator=(Cairo::RefPtr<CairoExtra>());
}

void
CairoWrapper::reset_surface(int w, int h){
  image=Image<int>();
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
