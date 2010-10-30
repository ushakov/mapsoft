#include "vmap_renderer.h"

const double VMAPRenderer::pics_dpi = 600.0;
const char * VMAPRenderer::pics_dir = "/usr/share/mapsoft/pics";

// create pattern from png-file, rescaled
// according to dpi and pics_dpi values and
// translated to the image center
Cairo::RefPtr<Cairo::SurfacePattern>
VMAPRenderer::get_patt_from_png(const char * fname){
  std::string file(pics_dir);
  file+="/";  file+=fname;
  try{
    Cairo::RefPtr<Cairo::ImageSurface> patt_surf =
      Cairo::ImageSurface::create_from_png(file.c_str());
    Cairo::RefPtr<Cairo::SurfacePattern> patt =
      Cairo::SurfacePattern::create(patt_surf);
    Cairo::Matrix M=Cairo::identity_matrix();
    M.translate(patt_surf->get_width()/2.0, patt_surf->get_height()/2.0);
    M.scale(pics_dpi/dpi, pics_dpi/dpi);
    patt->set_matrix(M);
    return patt;
  }
  catch (Cairo::logic_error err){
    std::cerr << "error: " << err.what() << " " << file << "\n";
    exit(1);
  }
}


void
VMAPRenderer::paintim(const vmap::object & o,
        const Cairo::RefPtr<Cairo::SurfacePattern> & patt, double ang){
  for (vmap::object::const_iterator l=o.begin(); l!=o.end(); l++){
    if (l->size()<1) continue;
    dPoint p=l->range().CNT();
    cr->save();
    cr->translate(p.x, p.y);
    cr->rotate(ang);
    cr->set_source(patt);
    cr->paint();
    cr->restore();
  }
}

// place image in the center of polygons
void
VMAPRenderer::render_im_in_polygons(int type, const char * fname){
  cr->save();
  Cairo::RefPtr<Cairo::SurfacePattern> patt =
    get_patt_from_png(fname);
  for (vmap::world::const_iterator o=W.begin(); o!=W.end(); o++){
    if (o->type!=(type | zn::area_mask)) continue;
    paintim(*o, patt);
  }
  cr->restore();
}

// place image in points
void
VMAPRenderer::render_im_in_points(int type, const char * fname){

  Cairo::RefPtr<Cairo::SurfacePattern> patt =
    get_patt_from_png(fname);
  for (vmap::world::const_iterator o=W.begin(); o!=W.end(); o++){
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
