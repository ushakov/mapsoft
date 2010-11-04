#include "vmap_renderer.h"
#include <string>
#include <sstream>
#include <iomanip>
#include <boost/lexical_cast.hpp>

using namespace std;

VMAPRenderer::VMAPRenderer(const char * in_file, int dpi_,
    int lm, int tm, int rm, int bm): dpi(dpi_){

  W=vmap::read(in_file);
  if (W.size()==0) exit(1);

  // try to detect border from nom_name
  // todo - use option set from vmap_filt.cpp
  dRect nom_range=convs::nom_to_range(W.name);
  if (nom_range.empty())  nom_range=convs::nom_to_range(in_file);
  if (!nom_range.empty()){
    convs::pt2pt nom_cnv(
      Datum("wgs84"), Proj("lonlat"), Options(),
      Datum("pulkovo"), Proj("lonlat"), Options());
    W.brd = nom_cnv.line_bck(rect2line(nom_range));
  }

  ref = vmap::mk_tmerc_ref(W, dpi/2.54, true);

  lw1 = dpi/105.0; // standard line width (1/105in?)
  fs1 = dpi/89.0;  // standard font size

  dRect rng = ref.border.range();

  ref+=dPoint(lm,tm);
  convs::map2pt cnv = convs::map2pt(ref, Datum("WGS84"), Proj("lonlat"), Options());

  rng.x = rng.y = 0;
  rng.w+=lm+rm; if (rng.w<0) rng.w=0;
  rng.h+=tm+bm; if (rng.h<0) rng.h=0;


  cerr
     << "  scale  = 1:" << int(W.rscale) << "\n"
     << "  dpi    = " << dpi << "\n"
     << "  image = " << int(rng.w) << "x" << int(rng.h)<< "\n";


  // create Cairo surface and context
  surface = Cairo::ImageSurface::create(
    Cairo::FORMAT_ARGB32, rng.w, rng.h);
  cr = Cairo::Context::create(surface);

  // antialiasing is not compatable with erasing
  // dark points under labels
  cr->set_antialias(Cairo::ANTIALIAS_NONE); 

  // draw border, set clipping region
  set_color(0xFFFFFF); cr->paint();
  for (dLine::const_iterator p=ref.border.begin(); p!=ref.border.end(); p++){
    if (p==ref.border.begin()) cr->move_to(p->x,p->y);
    else cr->line_to(p->x,p->y);
  }
  cr->close_path();
  cr->save();
  cr->set_source_rgb(0,0,0);
  cr->set_line_width(6); // draw border
  cr->stroke_preserve();
  cr->restore();
  cr->clip_preserve();   // set clipping region
  cr->save();
  cr->set_source_rgb(1,1,1); // erase border inside clip region
  cr->set_line_width(10);
  cr->stroke();
  cr->restore();

  // modify vmap
  vmap::add_labels(W);
  vmap::move_pics(W);

  // convert coordinates to px
  for (vmap::world::iterator o=W.begin(); o!=W.end(); o++){
    // convert object angles: deg (latlon) -> rad (raster)
    if (o->opts.exists("Angle")){
      double a = o->opts.get<double>("Angle",0);
      a = cnv.ang_bck(o->center(), M_PI/180 * a, 0.01);
      o->opts.put<double>("Angle", a);
    }
    // convert object coords
    for (vmap::object::iterator l=o->begin(); l!=o->end(); l++){
      for (dLine::iterator p=l->begin(); p!=l->end(); p++){
        cnv.bck(*p);
      }
    }
    // convert label angles: deg (latlon) -> rad (raster) and pos.
    for (list<vmap::lpos>::iterator l=o->labels.begin(); l!=o->labels.end(); l++){
      if (!l->hor) l->ang = cnv.ang_bck(l->pos, M_PI/180 * l->ang, 0.01);
      cnv.bck(l->pos);
    }
  }
  // convert border
  for (dLine::iterator p=W.brd.begin(); p!=W.brd.end(); p++){
    cnv.bck(*p);
  }
  if (W.size() == 0){
    cerr << "warning: no objects\n";
  }
  if (rect_intersect(W.range(), ref.border.range()).empty()){
    cerr << "warning: map outside its border\n";
  }
}

void
VMAPRenderer::set_color(int c){
  cr->set_source_rgb(
    ((c&0xFF0000)>>16)/256.0,
    ((c&0xFF00)>>8)/256.0,
    (c&0xFF)/256.0
  );
}
void
VMAPRenderer::set_th(double th){
  cr->set_line_width(th*lw1);;
}
void
VMAPRenderer::unset_dash(){
  cr->unset_dash();
}
void
VMAPRenderer::set_dash(double d1, double d2){
  vector<double> d;
  d.push_back(d1*lw1);
  d.push_back(d2*lw1);
  cr->set_dash(d, 0);
}
void
VMAPRenderer::set_dash(double d1, double d2, double d3, double d4){
  vector<double> d;
  d.push_back(d1*lw1);
  d.push_back(d2*lw1);
  d.push_back(d3*lw1);
  d.push_back(d4*lw1);
  cr->set_dash(d, 0);
}
void
VMAPRenderer::set_cap_round(){
  cr->set_line_cap(Cairo::LINE_CAP_ROUND);
}
void
VMAPRenderer::set_cap_butt(){
  cr->set_line_cap(Cairo::LINE_CAP_BUTT);
}
void
VMAPRenderer::set_cap_square(){
  cr->set_line_cap(Cairo::LINE_CAP_SQUARE);
}
void
VMAPRenderer::set_join_miter(){
  cr->set_line_join(Cairo::LINE_JOIN_MITER);
}
void
VMAPRenderer::set_join_round(){
  cr->set_line_join(Cairo::LINE_JOIN_ROUND);
}

void
VMAPRenderer::mkpath(const vmap::object & o, const int close, double curve_l){
  curve_l*=lw1;
  for (vmap::object::const_iterator l=o.begin(); l!=o.end(); l++){
    if (l->size()<1) continue;
    dPoint old;
    bool first = true;
    for (dLine::const_iterator p=l->begin(); p!=l->end(); p++){
      if (p==l->begin()){
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
VMAPRenderer::mkptpath(const vmap::object & o){
  for (vmap::object::const_iterator l=o.begin(); l!=o.end(); l++){
    if (l->size()<1) continue;
    dPoint p = *l->begin();
    cr->move_to(p.x, p.y);
    cr->rel_line_to(0,0);
  }
}


void
VMAPRenderer::render_polygons(int type, int col, double curve_l){
  cr->save();
  set_color(col);
  for (vmap::world::const_iterator o=W.begin(); o!=W.end(); o++){
    if (o->type!=(type | zn::area_mask)) continue;
    mkpath(*o, 1, curve_l);
    cr->fill();
  }
  cr->restore();
}

// contoured polygons
void
VMAPRenderer::render_polygons(int type, int fill_col, int cnt_col,
                              double cnt_th, double curve_l){
  cr->save();
  set_color(fill_col);
  for (vmap::world::const_iterator o=W.begin(); o!=W.end(); o++){
    if (o->type!= (type | zn::area_mask)) continue;
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

// polygons filled with image pattern
void
VMAPRenderer::render_polygons(int type, const char * fname, double curve_l){
  cr->save();
  Cairo::RefPtr<Cairo::SurfacePattern> patt =
    get_patt_from_png(fname);
  patt->set_extend(Cairo::EXTEND_REPEAT);
  cr->set_source(patt);
  for (vmap::world::const_iterator o=W.begin(); o!=W.end(); o++){
    if (o->type!=(type | zn::area_mask)) continue;
    mkpath(*o, 1, curve_l);
    cr->fill();
  }
  cr->restore();
}


void
VMAPRenderer::render_line(int type, int col, double th, double curve_l){
  cr->save();
  set_th(th);
  set_color(col);
  for (vmap::world::const_iterator o=W.begin(); o!=W.end(); o++){
    if (o->type!=(type | zn::line_mask)) continue;
    mkpath(*o, 0, curve_l);
  }
  cr->stroke();
  cr->restore();
}

void
VMAPRenderer::render_points(int type, int col, double th){
  cr->save();
  set_th(th);
  set_color(col);
  for (vmap::world::const_iterator o=W.begin(); o!=W.end(); o++){
    if (o->type!=type) continue;
    mkptpath(*o);
  }
  cr->stroke();
  cr->restore();
}



// paths for bridge sign
void
VMAPRenderer::mkbrpath1(const vmap::object & o){
  for (vmap::object::const_iterator l=o.begin(); l!=o.end(); l++){
    if (l->size()<2) continue;
    dPoint p1 = (*l)[0], p2 = (*l)[1];
    cr->move_to(p1.x, p1.y);
    cr->line_to(p2.x, p2.y);
  }
}
void
VMAPRenderer::mkbrpath2(const vmap::object & o, double th, double side){
  th*=lw1/2.0;
  side*=lw1/sqrt(2.0);
  for (vmap::object::const_iterator l=o.begin(); l!=o.end(); l++){
    if (l->size()<2) continue;
    dPoint p1 = (*l)[0], p2 = (*l)[1];
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
VMAPRenderer::render_bridge(int type, double th1, double th2, double side){
  cr->save();
  cr->set_line_cap(Cairo::LINE_CAP_BUTT);
  cr->set_line_join(Cairo::LINE_JOIN_ROUND);
  if (th1!=0){
    set_th(th1);
    set_color(0xFFFFFF);
    for (vmap::world::const_iterator o=W.begin(); o!=W.end(); o++){
      if (o->type!=(type | zn::line_mask)) continue;
      mkbrpath1(*o);
    }
    cr->stroke();
    th1+=th2;
  }
  set_th(th2);
  set_color(0x0);
  for (vmap::world::const_iterator o=W.begin(); o!=W.end(); o++){
    if (o->type!=(type | zn::line_mask)) continue;
    mkbrpath2(*o, th1, side);
  }
  cr->stroke();
  cr->restore();
}


void
VMAPRenderer::render_line_el(int type, int col, double th, double step){
  render_line(type, col, th, 0);
  double width=th*1.2*lw1;
  step*=lw1;

  cr->save();
  set_cap_round();
  set_th(1.8);
  set_color(col);
  for (vmap::world::const_iterator o=W.begin(); o!=W.end(); o++){
    if (o->type!=(type | zn::line_mask)) continue;
    for (vmap::object::const_iterator l=o->begin(); l!=o->end(); l++){
      LineDist ld(*l);
      if (ld.length()<=step) continue;
      double fstep = ld.length()/ceil(ld.length()/step);
      ld.move_frw(fstep/2);
      int n=1;
      while (ld.dist() < ld.length()){
        dPoint vn=ld.norm()*width;
        dPoint vt=ld.tang()*width;
        dPoint p1,p2,p3;
        p1=p2=p3=ld.pt();
        p1+=vn; p3-=vn;
        if (n%4 == 1){ p1+=vt; p3+=vt;}
        if (n%4 == 3){ p1-=vt; p3-=vt;}
        cr->move_to(p1.x, p1.y);
        cr->line_to(p2.x, p2.y);
        cr->line_to(p3.x, p3.y);
        n++;
        ld.move_frw(fstep);
      }
    }
  }
  cr->stroke();
  cr->restore();
}

void
VMAPRenderer::render_line_obr(int type, int col, double th){
  render_line(type, col, th, 0);
  double width=th*2*lw1;
  double step=th*4*lw1;

  cr->save();
  set_cap_round();
  set_th(th);
  set_color(col);

  for (vmap::world::const_iterator o=W.begin(); o!=W.end(); o++){
    if (o->type!=(type | zn::line_mask)) continue;
    int k=1;
    if (o->dir==2) k=-1;
    for (vmap::object::const_iterator l=o->begin(); l!=o->end(); l++){
      LineDist ld(*l);
      if (ld.length()<=step) continue;
      double fstep = ld.length()/ceil(ld.length()/step);
      ld.move_frw(fstep/2);
      while (ld.dist() < ld.length()){
       dPoint p=ld.pt(), vn=ld.norm()*width*k;
        cr->move_to(p.x, p.y);
        cr->line_to(p.x+vn.x, p.y+vn.y);
        ld.move_frw(fstep);
      }
    }
  }
  cr->stroke();
  cr->restore();
}

void
VMAPRenderer::render_line_zab(int type, int col, double th){
  render_line(type, col, th, 0);
  double width=th*2*lw1;
  double step=th*8*lw1;

  cr->save();
  set_cap_round();
  set_th(th);
  set_color(col);

  for (vmap::world::const_iterator o=W.begin(); o!=W.end(); o++){
    if (o->type!=(type | zn::line_mask)) continue;
    int k=1;
    if (o->dir==2) k=-1;
    for (vmap::object::const_iterator l=o->begin(); l!=o->end(); l++){
      LineDist ld(*l);
      if (ld.length()<=step) continue;
      double fstep = ld.length()/ceil(ld.length()/step);
      ld.move_frw((fstep-width)/2);
      int n=0;
      while (ld.dist() < ld.length()){
       dPoint p=ld.pt(), v=(ld.norm()-ld.tang())*width*k;
        cr->move_to(p.x, p.y);
        cr->line_to(p.x+v.x, p.y+v.y);
        ld.move_frw((n%2==0)?width:fstep-width);
        n++;
      }
    }
  }
  cr->stroke();
  cr->restore();
}

void
VMAPRenderer::render_line_gaz(int type, int col, double th, double step){
  render_line(type, col, th, 0);
  double width=th*0.8*lw1;
  step*=lw1;

  cr->save();
  set_th(1);
  for (vmap::world::const_iterator o=W.begin(); o!=W.end(); o++){
    if (o->type!=(type | zn::line_mask)) continue;
    for (vmap::object::const_iterator l=o->begin(); l!=o->end(); l++){
      LineDist ld(*l);
      if (ld.length()<=step) continue;
      double fstep = ld.length()/ceil(ld.length()/step);
      ld.move_frw(fstep/2);
      while (ld.dist() < ld.length()){
        dPoint p=ld.pt();
        cr->begin_new_sub_path();
        cr->arc(p.x, p.y, width, 0.0, 2*M_PI);
        ld.move_frw(fstep);
      }
    }
  }
  set_color(0xFFFFFF);
  cr->fill_preserve();
  set_color(col);
  cr->stroke();
  cr->restore();
}

void
VMAPRenderer::save_image(const char * png, const char * map){
  if (png) surface->write_to_png(png);
  if (!map) return;
  g_map M = ref;
  M.file = png;
  ofstream f(map);
  oe::write_map_file(f, M, Options());
}

void
VMAPRenderer::render_grid_label(double c, double val, bool horiz){

  ostringstream ss;
  ss << setprecision(7) << val/1000.0;

  if (ref.border.size()<1) return;

  dPoint pmin, pmax;
  double amin=0, amax=0;
  for (int i=1; i<ref.border.size(); i++){
    dPoint p1(ref.border[i-1]);
    dPoint p2(ref.border[i]);
    if (horiz){
      if (p1.x == p2.x) continue;
      if (p1.x > p2.x) p1.swap(p2);
      // segment does not cross grid line:
      if ((p1.x >= c) || (p2.x < c)) continue;
      double a = atan2(p2.y-p1.y, p2.x-p1.x);

      // crossing point of grid line with border
      dPoint pc(c, p1.y + (p2.y-p1.y)*(c-p1.x)/(p2.x-p1.x));
      if ((pmin==dPoint()) || (pmin.y > pc.y)) {pmin=pc; amin=a;}
      if ((pmax==dPoint()) || (pmax.y < pc.y)) {pmax=pc; amax=a;}
    }
    else {
      if (p1.y == p2.y) continue;
      if (p1.y > p2.y) p1.swap(p2);
      // segment does not cross grid line:
      if ((p1.y >= c) || (p2.y < c)) continue;
      double a = atan2(p2.y-p1.y, p2.x-p1.x);

      // crossing point of grid line with border
      dPoint pc(p1.x + (p2.x-p1.x)*(c-p1.y)/(p2.y-p1.y), c);
      if ((pmin==dPoint()) || (pmin.x > pc.x)) {pmin=pc; amin=a;}
      if ((pmax==dPoint()) || (pmax.x < pc.x)) {pmax=pc; amax=a;}
    }
  }

  if (amin>M_PI/2) amin-=M_PI;
  if (amin<-M_PI/2) amin+=M_PI;
  if (amax>M_PI/2) amax-=M_PI;
  if (amax<-M_PI/2) amax+=M_PI;

  bool drawmin, drawmax;
  int ydir_max=0, ydir_min=0;

  if (horiz){
    drawmin = (pmin!=dPoint()) && (abs(amin) < M_PI* 0.1);
    drawmax = (pmax!=dPoint()) && (abs(amax) < M_PI* 0.1);
    pmin+=dPoint(0,-dpi/30);
    pmax+=dPoint(0,+dpi/30);
    ydir_max=2;
  }
  else{
    if (amin>0) amin-=M_PI;
    if (amax<0) amax+=M_PI;
    drawmin = (pmin!=dPoint()) && (abs(amin) > M_PI* 0.4);
    drawmax = (pmax!=dPoint()) && (abs(amax) > M_PI* 0.4);
    pmin+=dPoint(-dpi/30,0);
    pmax+=dPoint(dpi/30,0);
  }
  if (drawmin)
    render_text(ss.str().c_str(), pmin, amin, 0, 18, 8, 1,ydir_min);
  if (drawmax)
    render_text(ss.str().c_str(), pmax, amax, 0, 18, 8, 1,ydir_max);
}

void
VMAPRenderer::render_pulk_grid(double dx, double dy, bool draw_labels){

  convs::map2pt cnv(ref, Datum("pulkovo"), Proj("tmerc"), convs::map_popts(ref));

  dRect rng_m = cnv.bb_frw(ref.border.range(), 1);

  dPoint pb(
    rng_m.x,
    rng_m.y
  );
  dPoint pe(
    rng_m.x+rng_m.w,
    rng_m.y+rng_m.h
  );
  dx *= W.rscale/100;
  dy *= W.rscale/100;
  double m=dy/10; // skip labels at distance m from horizontal edges

  dPoint p(
    dx * floor(rng_m.x/dx),
    dy * floor(rng_m.y/dy)
  );
  dPoint pbc(pb); cnv.bck(pbc);
  dPoint pec(pe); cnv.bck(pec);
  // note: pb.y < pe.y, but pbc.y > pec.y!

  cr->save();
  cr->set_source_rgba(0,0,0,0.5);
  cr->set_line_width(2);
  while (p.x<pe.x){
    dPoint pc(p); cnv.bck(pc);
    cr->move_to(pc.x, pbc.y);
    cr->line_to(pc.x, pec.y);
    if (draw_labels) render_grid_label(pc.x, p.x, true);
    p.x+=dx;
  }
  while (p.y<pe.y){
    dPoint pc(p); cnv.bck(pc);
    cr->move_to(pbc.x, pc.y);
    cr->line_to(pec.x, pc.y);
    if (draw_labels && (p.y > pb.y+m) && (p.y<pe.y-m))
      render_grid_label(pc.y, p.y, false);
    p.y+=dy;
  }
  cr->stroke();
  cr->restore();
}

