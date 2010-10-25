#include "vmap_renderer.h"

// convert coordinates from meters to pixels
void VMAPRenderer::pt_m2pt(dPoint & p){
  p.x = p.x*m2pt - rng_pt.x;
  p.y = rng_pt.y + rng_pt.h - p.y*m2pt;
}
//  void pt_pt2m(dPoint & p){
//    p.x = (p.x+rng_pt.x)/m2pt;
//    p.y = (rng_pt.y + rng_pt.h - p.y)/m2pt;
//  }

VMAPRenderer::VMAPRenderer(const char * in_file, int dpi_):
      cnv(Datum("wgs84"), Proj("lonlat"), Options(),
          Datum("wgs84"), Proj("lonlat"), Options()){
  std::cerr << "Reading " << in_file << "\n";

  W=vmap::read(in_file);
  if (W.size()==0){
    exit(1);
  }

  dpi=dpi_;
  Options proj_opts;
  D = Datum("pulkovo");
  P = Proj("tmerc");

  double lon0=convs::lon2lon0(W.range().CNT().x);
  proj_opts.put("lon0", lon0);

  cnv = convs::pt2pt(D, P, proj_opts,
    Datum("WGS84"), Proj("lonlat"), Options());

  m2pt=100.0/W.rscale * dpi / 2.54;
  lw1 = dpi/105.0; // standard line width (1/105in?)
  fs1 = dpi/89.0;  // standard font size

  // try find range from name
  dRect nom_r = convs::nom_to_range(W.name);
  if (!nom_r.empty()){
    convs::pt2pt cnv_p(Datum("pulk"), Proj("lonlat"), Options(),
                       Datum("wgs84"), Proj("lonlat"), Options());
    brd = cnv.line_bck(cnv_p.line_frw(rect2line(nom_r)));
    rng_m = brd.range();
  }
  else{
    rng_m = cnv.bb_bck(W.range());
    brd = rect2line(rng_m);
  }
  rng_m = rect_pump(rng_m, 3/m2pt);

  rng_pt = rng_m*m2pt;

  std::cerr
     << "  datum  = " << D << "\n"
     << "  proj   = " << P << "\n"
     << "  lon0   = " << lon0 << "\n"
     << "  range  = " << iRect(rng_m) << "\n"
     << "  scale  = 1:" << W.rscale << "\n"
     << "  dpi    = " << dpi << "\n"
     << "  m2pt   = " << m2pt << "\n"
     << "  image = " << int(rng_pt.w) << "x" << int(rng_pt.h)<< "\n";


  // create Cairo surface and context
  surface = Cairo::ImageSurface::create(
    Cairo::FORMAT_ARGB32, rng_pt.w, rng_pt.h);
  cr = Cairo::Context::create(surface);

  // antialiasing is not compatable with erasing
  // dark points under labels
  cr->set_antialias(Cairo::ANTIALIAS_NONE); 

  // draw border, set clipping region
  set_color(0xFFFFFF); cr->paint();
  for (dLine::const_iterator p=brd.begin(); p!=brd.end(); p++){
    dPoint pc=*p; pt_m2pt(pc);
    if (p==brd.begin()) cr->move_to(pc.x,pc.y);
    else cr->line_to(pc.x,pc.y);
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
        pt_m2pt(*p);
      }
    }
    // convert label angles: deg (latlon) -> rad (raster) and pos.
    for (std::list<vmap::lpos>::iterator l=o->labels.begin(); l!=o->labels.end(); l++){
      if (!l->hor) l->ang = cnv.ang_bck(l->pos, M_PI/180 * l->ang, 0.01);
      cnv.bck(l->pos);
      pt_m2pt(l->pos);
    }
  }
  // convert border
  for (dLine::iterator p=W.brd.begin(); p!=W.brd.end(); p++){
    cnv.bck(*p);
    pt_m2pt(*p);
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
  std::vector<double> d;
  d.push_back(d1*lw1);
  d.push_back(d2*lw1);
  cr->set_dash(d, 0);
}
void
VMAPRenderer::set_dash(double d1, double d2, double d3, double d4){
  std::vector<double> d;
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
  g_map M;
  M.file = png;
  M.comm = W.name;
  M.map_proj = P;
  for (dLine::const_iterator p=brd.begin(); p!=brd.end(); p++){
    dPoint gp = *p, rp = *p;
    // small negative values brokes map-file
    if (rp.x<0) rp.x=0;
    if (rp.y<0) rp.y=0;
    M.border.push_back(rp);
    M.push_back(g_refpoint(gp, rp));
  }
  std::ofstream f(map);
  oe::write_map_file(f, M, Options());
}

void
VMAPRenderer::render_grid(double dx, double dy){
  dPoint pb(
    rng_m.x,
    rng_m.y
  );
  dPoint pe(
    rng_m.x+rng_m.w,
    rng_m.y+rng_m.h
  );
  dPoint p(
    dx * floor(rng_m.x/dx),
    dy * floor(rng_m.y/dy)
  );
  dPoint pbc(pb); pt_m2pt(pbc);
  dPoint pec(pe); pt_m2pt(pec);
  // note: pb.y < pe.y, but pbc.y > pec.y!

  cr->save();
  cr->set_source_rgba(0,0,0,0.5);
  cr->set_line_width(2);
  while (p.x<pe.x){
    dPoint pc(p); pt_m2pt(pc);
    cr->move_to(pc.x, pbc.y);
    cr->line_to(pc.x, pec.y);
    p.x+=dx;
  }
  while (p.y<pe.y){
    dPoint pc(p); pt_m2pt(pc);
    cr->move_to(pbc.x, pc.y);
    cr->line_to(pec.x, pc.y);
    p.y+=dy;
  }
  cr->stroke();
  cr->restore();
}

