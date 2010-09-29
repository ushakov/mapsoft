#ifndef MP_RENDERER
#define MP_RENDERER

#include <string>
#include <iostream>
#include <list>
#include <cairomm/cairommconfig.h>
#include <cairomm/context.h>
#include <cairomm/surface.h>

#include <libgeo/geo_convs.h>
#include <libmp/mp.h>

const double img_dpi = 300.0;

struct MPRenderer{

  Cairo::RefPtr<Cairo::ImageSurface> surface;
  Cairo::RefPtr<Cairo::Context> cr;
  mp::mp_world W;
  convs::pt2pt cnv;
  double rscale, dpi, m2pt, lw1;
  dRect rng;

  MPRenderer(const char * in_file):
        cnv(Datum("wgs84"), Proj("lonlat"), Options(),
            Datum("wgs84"), Proj("lonlat"), Options()){
    std::cerr << "Reading " << in_file << "\n";
    if (!mp::read(in_file, W)) exit(1);

    surface = Cairo::ImageSurface::create(Cairo::FORMAT_ARGB32, rng.w, rng.h);
    cr = Cairo::Context::create(surface);

    Options proj_opts;
    rscale = 50000;
    dpi = 200;
    Datum D("pulkovo");
    Proj  P("tmerc");

    double lon0=convs::lon2lon0(W.range().CNT().x);
    proj_opts.put("lon0", lon0);

    cnv = convs::pt2pt(Datum("WGS84"), Proj("lonlat"), Options(), D, P, proj_opts);
    rng=cnv.bb_frw(W.range());

    m2pt=100.0/rscale * dpi / 2.54;
    lw1 = dpi/100; // standard line width (1/80in)

    std::cerr
       << "  datum  = " << D << "\n"
       << "  proj   = " << P << "\n"
       << "  lon0   = " << lon0 << "\n"
       << "  range  = " << iRect(rng) << "\n"
       << "  scale  = 1:" << rscale << "\n"
       << "  dpi    = " << dpi << "\n"
       << "  m2pt   = " << m2pt << "\n"
       << "  image = " << int(rng.w*m2pt) << "x" << int(rng.h*m2pt)<< "\n";

    rng*=m2pt;

    surface = Cairo::ImageSurface::create(Cairo::FORMAT_ARGB32, rng.w, rng.h);
    cr = Cairo::Context::create(surface);

    cr->set_identity_matrix();
    set_color(0xFFFFFF); cr->paint();
  }

  void set_color(int c){
    cr->set_source_rgb(
      ((c&0xFF0000)>>16)/256.0,
      ((c&0xFF00)>>8)/256.0,
      (c&0xFF)/256.0
    );
  }
  void set_th(double th){
    cr->set_line_width(th*lw1);;
  }
  void
  unset_dash(){
    cr->unset_dash();
  }
  void
  set_dash(double d1, double d2){
    std::vector<double> d;
    d.push_back(d1*lw1);
    d.push_back(d2*lw1);
    cr->set_dash(d, 0);
  }
  void
  set_dash(double d1, double d2, double d3, double d4){
    std::vector<double> d;
    d.push_back(d1*lw1);
    d.push_back(d2*lw1);
    d.push_back(d3*lw1);
    d.push_back(d4*lw1);
    cr->set_dash(d, 0);
  }
  void
  set_cap_round(){
    cr->set_line_cap(Cairo::LINE_CAP_ROUND);
  }
  void
  set_cap_butt(){
    cr->set_line_cap(Cairo::LINE_CAP_BUTT);
  }
  void
  set_cap_square(){
    cr->set_line_cap(Cairo::LINE_CAP_SQUARE);
  }
  void
  set_join_miter(){
    cr->set_line_join(Cairo::LINE_JOIN_MITER);
  }
  void
  set_join_round(){
    cr->set_line_join(Cairo::LINE_JOIN_ROUND);
  }


  Cairo::RefPtr<Cairo::SurfacePattern>
  get_patt_from_png(const char * fname){
    Cairo::RefPtr<Cairo::ImageSurface> patt_surf =
      Cairo::ImageSurface::create_from_png(fname);
    Cairo::RefPtr<Cairo::SurfacePattern> patt =
      Cairo::SurfacePattern::create(patt_surf);
    Cairo::Matrix M=Cairo::identity_matrix();
    M.translate(patt_surf->get_width()/2.0, patt_surf->get_height()/2.0);
    M.scale(img_dpi/dpi, img_dpi/dpi);
    patt->set_matrix(M);
    return patt;
  }

  void
  mkpath(const mp::mp_object & o, const int close, double curve_l=0){
    curve_l*=lw1;
    for (mp::mp_object::const_iterator l=o.begin(); l!=o.end(); l++){
      if (l->size()<1) continue;
      dLine l1 = cnv.line_frw(*l);
      dPoint old;
      bool first = true;
      for (dLine::iterator p=l1.begin(); p!=l1.end(); p++){
        *p *= m2pt;
        p->x = p->x - rng.x; p->y=rng.y + rng.h - p->y;

        if (p==l1.begin()){
          cr->move_to(p->x, p->y);
          old=*p;
          continue;
        }
        if (curve_l==0){
          cr->line_to(p->x, p->y);
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
            cr->curve_to(old.x, old.y, old.x, old.y, p1.x, p1.y);
          }
          else {
            first=false;
          }
          cr->line_to(p2.x, p2.y);
        }
        old=*p;
      }
      if (curve_l!=0)  cr->line_to(old.x, old.y);
      if (close) cr->close_path();
    }
  }

  void
  mkptpath(const mp::mp_object & o){
    for (mp::mp_object::const_iterator l=o.begin(); l!=o.end(); l++){
      if (l->size()<1) continue;
      dPoint p = *l->begin();
      cnv.frw(p);
      p *= m2pt;
      p.x = p.x - rng.x; p.y=rng.y + rng.h - p.y;
      cr->move_to(p.x, p.y);
      cr->rel_line_to(0,0);
    }
  }

  void
  paintim(const mp::mp_object & o, const Cairo::RefPtr<Cairo::SurfacePattern> & patt){
    for (mp::mp_object::const_iterator l=o.begin(); l!=o.end(); l++){
      if (l->size()<1) continue;
      dLine l1 = cnv.line_frw(*l);
      for (dLine::iterator p=l1.begin(); p!=l1.end(); p++){
        *p *= m2pt;
        p->x = p->x - rng.x; p->y=rng.y + rng.h - p->y;
      }
      dPoint p=l1.range().CNT();
      cr->save();
      cr->translate(p.x, p.y);
      cr->set_source(patt);
      cr->paint();
      cr->restore();
    }
  }


  void
  render_col_filled_polygons(int type, int col, double curve_l=0){
    cr->save();
    set_color(col);
    for (mp::mp_world::const_iterator o=W.begin(); o!=W.end(); o++){
      if (o->Type!=type) continue;
      if (o->Class!="POLYGON") continue;
      mkpath(*o, 1, curve_l);
      cr->fill();
    }
    cr->restore();
  }

  // polygons filled with image pattern
  void
  render_im_filled_polygons(int type, const char * fname, double curve_l=0){
    cr->save();
    Cairo::RefPtr<Cairo::SurfacePattern> patt =
      get_patt_from_png(fname);
    patt->set_extend(Cairo::EXTEND_REPEAT);
    cr->set_source(patt);
    for (mp::mp_world::const_iterator o=W.begin(); o!=W.end(); o++){
      if (o->Type!=type) continue;
      if (o->Class!="POLYGON") continue;
      mkpath(*o, 1, curve_l);
      cr->fill();
    }
    cr->restore();
  }

  // place image in the center of polygons
  void
  render_im_in_polygons(int type, const char * fname){
    cr->save();
    Cairo::RefPtr<Cairo::SurfacePattern> patt =
      get_patt_from_png(fname);
    for (mp::mp_world::const_iterator o=W.begin(); o!=W.end(); o++){
      if (o->Type!=type) continue;
      if (o->Class!="POLYGON") continue;
      paintim(*o, patt);
    }
    cr->restore();
  }
  // place image in points
  void
  render_im_in_points(int type, const char * fname){
    cr->save();
    Cairo::RefPtr<Cairo::SurfacePattern> patt =
      get_patt_from_png(fname);
    for (mp::mp_world::const_iterator o=W.begin(); o!=W.end(); o++){
      if (o->Type!=type) continue;
      if (o->Class!="POI") continue;
      paintim(*o, patt);
    }
    cr->restore();
  }

  // contoured polygons
  void
  render_cnt_polygons(int type, int fill_col, int cnt_col, double cnt_th, double curve_l=0){
    cr->save();
    set_color(fill_col);
    for (mp::mp_world::const_iterator o=W.begin(); o!=W.end(); o++){
      if (o->Type!=type) continue;
      if (o->Class!="POLYGON") continue;
      mkpath(*o, 1, curve_l);
      cr->fill_preserve();
      cr->save();
      set_color(cnt_col);
      set_th(cnt_th);
      cr->stroke();
      cr->restore();
    }
    cr->restore();
  }


  void
  render_line(int type, int col, double th, double curve_l){
    cr->save();
    set_th(th);
    set_color(col);
    for (mp::mp_world::const_iterator o=W.begin(); o!=W.end(); o++){
      if (o->Type!=type) continue;
      if (o->Class!="POLYLINE") continue;
      mkpath(*o, 0, curve_l);
    }
    cr->stroke();
    cr->restore();
  }

  void
  render_points(int type, int col, double th){
    cr->save();
    set_th(th);
    set_color(col);
    for (mp::mp_world::const_iterator o=W.begin(); o!=W.end(); o++){
      if (o->Type!=type) continue;
      if (o->Class!="POI") continue;
      mkptpath(*o);
    }
    cr->stroke();
    cr->restore();
  }

// CONTOURS

  void
  draw_cnt(const std::list<iPoint> & cnt, int c, double th){
    cr->save();
    cr->begin_new_path();
    set_th(th);
    set_color(c);
    cr->set_line_cap(Cairo::LINE_CAP_ROUND);

    for ( std::list<iPoint>::const_iterator p=cnt.begin(); p!=cnt.end(); p++){
      cr->move_to(p->x, p->y);
      cr->rel_line_to(0, 0);
    }
    cr->stroke();
    cr->restore();
  }


  std::list<iPoint>
  make_cnt(int c, double dist){
    dist*=lw1;

    unsigned char *data=surface->get_data();
    int w=surface->get_width();
    int h=surface->get_height();
    int s=surface->get_stride();
#define COL(x,y)  ((data[s*(y) + 4*(x) + 2] << 16)\
                 + (data[s*(y) + 4*(x) + 1] << 8)\
                 +  data[s*(y) + 4*(x) + 0])
#define ADJ(x,y,i)  COL( x+ ((i==1)?1:0) - ((i==3)?1:0), y + ((i==2)?1:0) - ((i==0)?1:0))
#define DIST2(x,y) ((x)*(x) + (y)*(y))
    std::list<iPoint> points;
    std::list<iPoint> ret;

    // walk through all non-border points
    for (int y=1; y<h-1; y++){
      for (int x=1; x<w-1; x++){
        bool mk_pt=false;
        if (COL(x,y) != c) continue;
        for (int k=0;k<4;k++){
          if (ADJ(x,y,k) != c) {
            mk_pt=true;
            break;
          }
        }
        if (!mk_pt) continue;


        std::list<iPoint>::iterator p=points.begin();
        while (p!=points.end()){
          if (DIST2(x - p->x, y - p->y) < dist*dist) {
            mk_pt=false;
            break;
          }
          if (abs(y - p->y) > dist) {
            p=points.erase(p);
          }
          else {
            p++;
          }
        }
        if (!mk_pt) continue;
        points.push_back(iPoint(x,y));
        ret.push_back(iPoint(x,y));
      }
    }
    return ret;
  }

  void
  filter_cnt(std::list<iPoint> & cnt, int c){
    unsigned char *data=surface->get_data();
    int w=surface->get_width();
    int h=surface->get_height();
    int s=surface->get_stride();
    std::list<iPoint>::iterator p=cnt.begin();
    while (p!=cnt.end()){
      int x=p->x, y=p->y;
      if ((x>=w) || (x<0) || (y>=h) || (y<0)) continue;
      if (COL(p->x, p->y)!=c) p=cnt.erase(p);
      else p++;
    }
  }

  void
  save_png(const char * out_file){
    surface->write_to_png(out_file);
  }

};

#endif