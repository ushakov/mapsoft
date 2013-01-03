#include "vmap_renderer.h"
#include "loaders/image_r.h"

const double VMAPRenderer::pics_dpi = 600.0;
const char * VMAPRenderer::pics_dir = "/usr/share/mapsoft/pics";

// create pattern from png-file, rescaled
// according to dpi and pics_dpi values and
// translated to the image center
Cairo::RefPtr<Cairo::SurfacePattern>
VMAPRenderer::get_patt_from_image(const iImage & I){
  try{
    const Cairo::Format format = Cairo::FORMAT_ARGB32;
    assert(Cairo::ImageSurface::format_stride_for_width(format, I.w) == I.w*4);
    Cairo::RefPtr<Cairo::ImageSurface> patt_surf =
      Cairo::ImageSurface::create((unsigned char*)I.data, format, I.w, I.h, I.w*4);
    Cairo::RefPtr<Cairo::SurfacePattern> patt =
      Cairo::SurfacePattern::create(patt_surf);
    Cairo::Matrix M=Cairo::identity_matrix();
    M.translate(patt_surf->get_width()/2.0, patt_surf->get_height()/2.0);
    M.scale(pics_dpi/dpi, pics_dpi/dpi);
    patt->set_matrix(M);
    return patt;
  }
  catch (Cairo::logic_error err){
    std::cerr << "error: " << err.what() << "\n";
    exit(1);
  }
}


// place image in the center of polygons
void
VMAPRenderer::render_im_in_polygons(int type, const char * fname){
  string f = string(pics_dir) + "/" + fname;
  iImage I = image_r::load(f.c_str());
  Cairo::RefPtr<Cairo::SurfacePattern> patt = get_patt_from_image(I);
  for (vmap::world::const_iterator o=W->begin(); o!=W->end(); o++){
    if (o->type!=(type | zn::area_mask)) continue;
    for (vmap::object::const_iterator l=o->begin(); l!=o->end(); l++){
      if (l->size()<1) continue;
      dPoint p=l->range().CNT();
      cr->save();
      cr->translate(p.x, p.y);
      cr->set_source(patt);
      cr->paint();
      cr->restore();
    }
  }
}

// polygons filled with image pattern
void
VMAPRenderer::render_img_polygons(int type, const char * fname, double curve_l){
  string f = string(pics_dir) + "/" + fname;
  iImage I = image_r::load(f.c_str());
  Cairo::RefPtr<Cairo::SurfacePattern> patt = get_patt_from_image(I);
  patt->set_extend(Cairo::EXTEND_REPEAT);
  cr->save();
  cr->set_source(patt);
  for (vmap::world::const_iterator o=W->begin(); o!=W->end(); o++){
    if (o->type!=(type | zn::area_mask)) continue;
    cr->mkpath_smline(*o, 1, curve_l*lw1);
    cr->fill();
  }
  cr->restore();
}

// place image in points
void
VMAPRenderer::render_im_in_points(int type, const char * fname){
  string f = string(pics_dir) + "/" + fname;
  iImage I = image_r::load(f.c_str());
  Cairo::RefPtr<Cairo::SurfacePattern> patt = get_patt_from_image(I);
  for (vmap::world::const_iterator o=W->begin(); o!=W->end(); o++){
    if (o->type!=type) continue;
    for (vmap::object::const_iterator l=o->begin(); l!=o->end(); l++){
      if (l->size()<1) continue;
      cr->save();
      cr->translate((*l)[0].x, (*l)[0].y);
      if (o->opts.exists("Angle"))
        cr->rotate(o->opts.get<double>("Angle",0));
      cr->set_source(patt);
      cr->paint();
      cr->restore();
    }
  }
}
