#ifndef MP_RENDERER
#define MP_RENDERER

#include <string>
#include <iostream>
#include <fstream>
#include <list>
#include <cairomm/cairommconfig.h>
#include <cairomm/context.h>
#include <cairomm/surface.h>

#include <libgeo/geo_convs.h>
#include <libgeo_io/io_oe.h>
#include <libmp/mp.h>

const double img_dpi = 300.0;

struct MPRenderer{

  Cairo::RefPtr<Cairo::ImageSurface> surface;
  Cairo::RefPtr<Cairo::Context> cr;
  mp::mp_world W;
  convs::pt2pt cnv;
  double rscale, dpi, m2pt, lw1;
  dRect rng_m, rng_pt;
  Datum D;
  Proj P;

  MPRenderer(const char * in_file):
        cnv(Datum("wgs84"), Proj("lonlat"), Options(),
            Datum("wgs84"), Proj("lonlat"), Options()){
    std::cerr << "Reading " << in_file << "\n";
    if (!mp::read(in_file, W)) exit(1);

    Options proj_opts;
    rscale = 50000;
    dpi = 200;
    D = Datum("pulkovo");
    P = Proj("tmerc");

    double lon0=convs::lon2lon0(W.range().CNT().x);
    proj_opts.put("lon0", lon0);

    cnv = convs::pt2pt(
      Datum("WGS84"), Proj("lonlat"), Options(), D, P, proj_opts);

    m2pt=100.0/rscale * dpi / 2.54;
    lw1 = dpi/80; // standard line width (1/80in)

    rng_m=cnv.bb_frw(W.range());
    rng_pt = rng_m*m2pt;

    std::cerr
       << "  datum  = " << D << "\n"
       << "  proj   = " << P << "\n"
       << "  lon0   = " << lon0 << "\n"
       << "  range  = " << iRect(rng_m) << "\n"
       << "  scale  = 1:" << rscale << "\n"
       << "  dpi    = " << dpi << "\n"
       << "  m2pt   = " << m2pt << "\n"
       << "  image = " << int(rng_pt.w) << "x" << int(rng_pt.h)<< "\n";

    surface = Cairo::ImageSurface::create(
      Cairo::FORMAT_ARGB32, rng_pt.w, rng_pt.h);
    cr = Cairo::Context::create(surface);
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

  // create pattern from png-file, rescaled
  // according to dpi and img_dpi values and
  // translated to the image center
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

  // convert coordinates from meters to pixels
  void pt_m2pt(dPoint & p){
    p.x = p.x*m2pt - rng_pt.x;
    p.y = rng_pt.y + rng_pt.h - p.y*m2pt;
  }
//  void pt_pt2m(dPoint & p){
//    p.x = (p.x+rng_pt.x)/m2pt;
//    p.y = (rng_pt.y + rng_pt.h - p.y)/m2pt;
//  }

  void
  mkpath(const mp::mp_object & o, const int close, double curve_l=0){
    curve_l*=lw1;
    for (mp::mp_object::const_iterator l=o.begin(); l!=o.end(); l++){
      if (l->size()<1) continue;
      dLine l1 = cnv.line_frw(*l);
      dPoint old;
      bool first = true;
      for (dLine::iterator p=l1.begin(); p!=l1.end(); p++){
        pt_m2pt(*p);
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

  // path for drawing points
  void
  mkptpath(const mp::mp_object & o){
    for (mp::mp_object::const_iterator l=o.begin(); l!=o.end(); l++){
      if (l->size()<1) continue;
      dPoint p = *l->begin();
      cnv.frw(p);
      pt_m2pt(p);
      cr->move_to(p.x, p.y);
      cr->rel_line_to(0,0);
    }
  }

  // paths for bridge sign
  void
  mkbrpath1(const mp::mp_object & o){
    for (mp::mp_object::const_iterator l=o.begin(); l!=o.end(); l++){
      if (l->size()<2) continue;
      dPoint p1 = (*l)[0], p2 = (*l)[1];
      cnv.frw(p1); cnv.frw(p2);
      pt_m2pt(p1); pt_m2pt(p2); 
      cr->move_to(p1.x, p1.y);
      cr->line_to(p2.x, p2.y);
    }
  }
  void
  mkbrpath2(const mp::mp_object & o, double th, double side){
    th*=lw1/2.0;
    side*=lw1/sqrt(2.0);
    for (mp::mp_object::const_iterator l=o.begin(); l!=o.end(); l++){
      if (l->size()<2) continue;
      dPoint p1 = (*l)[0], p2 = (*l)[1];
      cnv.frw(p1); cnv.frw(p2);
      pt_m2pt(p1); pt_m2pt(p2);
      if (th >= lw1*3){
        cr->move_to(p1.x, p1.y);
        cr->line_to(p2.x, p2.y);
      }
      dPoint t = pnorm(p2-p1);
      dPoint n(-t.y, t.x);
      dPoint P;
      P = p1 + th*n + side*(n-t); cr->move_to(P.x, P.y);
      P = p1 + th*n;              cr->line_to(P.x, P.y);
      P = p2 + th*n;              cr->line_to(P.x, P.y);
      P = p2 + th*n + side*(n+t); cr->line_to(P.x, P.y);

      P = p1 - th*n - side*(n+t); cr->move_to(P.x, P.y);
      P = p1 - th*n;              cr->line_to(P.x, P.y);
      P = p2 - th*n;              cr->line_to(P.x, P.y);
      P = p2 - th*n - side*(n-t); cr->line_to(P.x, P.y);
    }
  }

  void
  paintim(const mp::mp_object & o, const Cairo::RefPtr<Cairo::SurfacePattern> & patt){
    for (mp::mp_object::const_iterator l=o.begin(); l!=o.end(); l++){
      if (l->size()<1) continue;
      dLine l1 = cnv.line_frw(*l);
      dPoint p=l1.range().CNT();
      pt_m2pt(p);
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

  void
  render_bridge(int type, double th1, double th2, double side){
    cr->save();
    cr->set_line_cap(Cairo::LINE_CAP_BUTT);
    cr->set_line_join(Cairo::LINE_JOIN_ROUND);
    set_th(th1);
    set_color(0xFFFFFF);
    for (mp::mp_world::const_iterator o=W.begin(); o!=W.end(); o++){
      if (o->Type!=type) continue;
      if (o->Class!="POLYLINE") continue;
      mkbrpath1(*o);
    }
    cr->stroke();
    set_th(th2);
    set_color(0x0);
    for (mp::mp_world::const_iterator o=W.begin(); o!=W.end(); o++){
      if (o->Type!=type) continue;
      if (o->Class!="POLYLINE") continue;
      mkbrpath2(*o, th1+th2, side);
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

  void
  save_map(const char * map, const char * png){
    g_map M;
    M.file = png;
    M.comm = W.Name;
    M.map_proj = P;
    dLine l = rect2line(rng_m);
    for (dLine::const_iterator p=l.begin(); p!=l.end(); p++){
      dPoint gp = *p, rp = *p;
      cnv.bck(gp); pt_m2pt(rp);
      // small negative values brokes map-file
      if (rp.x<0) rp.x=0;
      if (rp.y<0) rp.y=0;
      M.border.push_back(rp);
      M.push_back(g_refpoint(gp, rp));
    }
    std::ofstream f(map);
    oe::write_map_file(f, M, Options());
  }

};

#endif