#include "vmap_renderer.h"
#include <string>
#include <sstream>
#include <iomanip>
#include <boost/lexical_cast.hpp>

using namespace std;

VMAPRenderer::VMAPRenderer(vmap::world * _W, int w, int h,
    const g_map & ref_, double dpi_, bool use_aa, bool transp):
      dpi(dpi_), W(_W), ref(ref_){

  lw1 = dpi/105.0; // standard line width (1/105in?)
  fs1 = dpi/89.0;  // standard font size

  convs::map2pt cnv = convs::map2pt(
    ref, Datum("WGS84"), Proj("lonlat"), Options());

  // create Cairo surface and context
  cr.reset_surface(w, h);

  // antialiasing is not compatable with erasing
  // dark points under labels
  if (!use_aa) cr->set_antialias(Cairo::ANTIALIAS_NONE); 

  dLine border=ref.border;
  if (W->brd.size()>2) border = cnv.line_bck(W->brd);

  // draw border, set clipping region
  if (!transp) {
    cr->set_color(0xFFFFFF);
    cr->paint();
  }
  for (dLine::const_iterator p=border.begin(); p!=border.end(); p++){
    if (p==ref.border.begin()) cr->move_to(*p);
    else cr->line_to(*p);
  }
  cr->close_path();
  cr->save();
  cr->set_source_rgb(0,0,0);
  cr->set_line_width(4); // draw border
  cr->stroke_preserve();
  cr->restore();
  cr->clip_preserve();   // set clipping region
  cr->save();
  cr->set_color(0xFFFFFF);
  cr->paint();
  cr->restore();

  // modify vmap
  vmap::join_labels(*W);
  vmap::move_pics(*W);

  // convert coordinates to px
  for (vmap::world::iterator o=W->begin(); o!=W->end(); o++){
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
      if (!l->hor) l->ang = -cnv.ang_bck(l->pos, -M_PI/180 * l->ang, 0.01);
      cnv.bck(l->pos);
    }
  }
  // convert border
  for (dLine::iterator p=W->brd.begin(); p!=W->brd.end(); p++){
    cnv.bck(*p);
  }
  if (W->size() == 0){
    cerr << "warning: no objects\n";
  }
  if (rect_intersect(W->range(), ref.border.range()).empty()){
    cerr << "warning: map outside its border\n";
  }
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

// path for drawing points
void
VMAPRenderer::mkptpath(const dMultiLine & o){
  for (vmap::object::const_iterator l=o.begin(); l!=o.end(); l++){
    if (l->size()<1) continue;
    dPoint p = *l->begin();
    cr->move_to(p);
    cr->rel_line_to(dPoint());
  }
}


void
VMAPRenderer::render_polygons(int type, int col, double curve_l){
  if (!cr) return;
  cr->save();
  cr->set_color(col);
  for (vmap::world::const_iterator o=W->begin(); o!=W->end(); o++){
    if (o->type!=(type | zn::area_mask)) continue;
    cr->mkpath_smline(*o, 1, curve_l*lw1);
    cr->fill();
  }
  cr->restore();
}

// contoured polygons
void
VMAPRenderer::render_cnt_polygons(int type, int fill_col, int cnt_col,
                              double cnt_th, double curve_l){
  cr->save();
  cr->set_color(fill_col);
  for (vmap::world::const_iterator o=W->begin(); o!=W->end(); o++){
    if (o->type!= (type | zn::area_mask)) continue;
    cr->mkpath_smline(*o, 1, curve_l*lw1);
    cr->fill_preserve();
    cr->save();
    cr->set_color(cnt_col);
    cr->set_line_width(cnt_th*lw1);
    cr->stroke();
    cr->restore();
  }
  cr->restore();
}

// polygons filled with image pattern
void
VMAPRenderer::render_img_polygons(int type, const char * fname, double curve_l){
  if (!cr) return;
  cr->save();
  Cairo::RefPtr<Cairo::SurfacePattern> patt =
    get_patt_from_png(fname);
  patt->set_extend(Cairo::EXTEND_REPEAT);
  cr->set_source(patt);
  for (vmap::world::const_iterator o=W->begin(); o!=W->end(); o++){
    if (o->type!=(type | zn::area_mask)) continue;
    cr->mkpath_smline(*o, 1, curve_l*lw1);
    cr->fill();
  }
  cr->restore();
}


void
VMAPRenderer::render_line(int type, int col, double th, double curve_l){
  if (!cr) return;
  cr->save();
  cr->set_line_width(th*lw1);
  cr->set_color(col);
  for (vmap::world::const_iterator o=W->begin(); o!=W->end(); o++){
    if (o->type!=(type | zn::line_mask)) continue;
    cr->mkpath_smline(*o, 0, curve_l*lw1);
  }
  cr->stroke();
  cr->restore();
}

void
VMAPRenderer::render_points(int type, int col, double th){
  if (!cr) return;
  cr->save();
  cr->set_line_width(th*lw1);
  cr->set_color(col);
  for (vmap::world::const_iterator o=W->begin(); o!=W->end(); o++){
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
    cr->move_to(p1);
    cr->line_to(p2);
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
    P = p1 + th*n + side*(n-t); cr->move_to(P);
    P = p1 + th*n;              cr->line_to(P);
    P = p2 + th*n;              cr->line_to(P);
    P = p2 + th*n + side*(n+t); cr->line_to(P);

    P = p1 - th*n - side*(n+t); cr->move_to(P);
    P = p1 - th*n;              cr->line_to(P);
    P = p2 - th*n;              cr->line_to(P);
    P = p2 - th*n - side*(n-t); cr->line_to(P);
  }
}
void
VMAPRenderer::render_bridge(int type, double th1, double th2, double side){
  cr->save();
  cr->set_line_cap(Cairo::LINE_CAP_BUTT);
  cr->set_line_join(Cairo::LINE_JOIN_ROUND);
  if (th1!=0){
    cr->set_line_width(th1*lw1);
    cr->set_color(0xFFFFFF);
    for (vmap::world::const_iterator o=W->begin(); o!=W->end(); o++){
      if (o->type!=(type | zn::line_mask)) continue;
      mkbrpath1(*o);
    }
    cr->stroke();
    th1+=th2;
  }
  cr->set_line_width(th2*lw1);
  cr->set_color(0x0);
  for (vmap::world::const_iterator o=W->begin(); o!=W->end(); o++){
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
  cr->set_line_width(1.8*lw1);
  cr->set_color(col);
  for (vmap::world::const_iterator o=W->begin(); o!=W->end(); o++){
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
        cr->move_to(p1);
        cr->line_to(p2);
        cr->line_to(p3);
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
  cr->set_line_width(th*lw1);
  cr->set_color(col);

  for (vmap::world::const_iterator o=W->begin(); o!=W->end(); o++){
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
        cr->move_to(p);
        cr->line_to(p+vn);
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
  cr->set_line_width(th*lw1);
  cr->set_color(col);

  for (vmap::world::const_iterator o=W->begin(); o!=W->end(); o++){
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
        cr->move_to(p);
        cr->line_to(p+v);
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
  cr->set_line_width(lw1);
  for (vmap::world::const_iterator o=W->begin(); o!=W->end(); o++){
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
  cr->set_color(0xFFFFFF);
  cr->fill_preserve();
  cr->set_color(col);
  cr->stroke();
  cr->restore();
}

void
VMAPRenderer::save_image(const char * png, const char * map){
  if (png) cr.get_im_surface()->write_to_png(png);
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
  dLine::const_iterator i,j;
  for (i=ref.border.begin(); i!=ref.border.end(); i++){
    j=i+1;
    if (j==ref.border.end()) j=ref.border.begin();
    dPoint p1(*i);
    dPoint p2(*j);
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
    drawmin = (pmin!=dPoint()) && (abs(amin) > M_PI* 0.4);
    drawmax = (pmax!=dPoint()) && (abs(amax) > M_PI* 0.4);
    if (amin>0) amin-=M_PI;
    if (amax<0) amax+=M_PI;
    pmin+=dPoint(-dpi/30,0);
    pmax+=dPoint(dpi/30,0);
  }
  if (drawmin)
    render_text(ss.str().c_str(), pmin, amin, 0, 18, 8, 1,ydir_min);
  if (drawmax)
    render_text(ss.str().c_str(), pmax, amax, 0, 18, 8, 1,ydir_max);
}

void
VMAPRenderer::render_pulk_grid(double dx, double dy, bool labels){

  if (ref.map_proj != Proj("tmerc")){
    cerr << "WARINIG: grid for non-tmerc maps is not supported!\n";
  }

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
  dx *= W->rscale/100;
  dy *= W->rscale/100;
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
    if (labels) render_grid_label(pc.x, p.x, true);
    else {
      cr->Cairo::Context::move_to(pc.x, pbc.y);
      cr->Cairo::Context::line_to(pc.x, pec.y);
    }
    p.x+=dx;
  }
  while (p.y<pe.y){
    dPoint pc(p); cnv.bck(pc);
    if (labels && (p.y > pb.y+m) && (p.y<pe.y-m))
      render_grid_label(pc.y, p.y, false);
    else {
      cr->Cairo::Context::move_to(pbc.x, pc.y);
      cr->Cairo::Context::line_to(pec.x, pc.y);
    }
    p.y+=dy;
  }
  cr->stroke();
  cr->restore();
}

void
VMAPRenderer::render_objects(const bool draw_contours){
    bool hr = (W->style == "hr");

  //*******************************

  render_polygons(0x16, 0xAAFFAA); // лес

  render_polygons(0x52, 0xFFFFFF); // поле
  render_polygons(0x15, 0xAAFFAA); // остров леса

  list<iPoint> cnt;
  if (draw_contours)
    cnt = make_cnt(0xAAFFAA, 2);     // контуры леса

  render_img_polygons(0x4f, "vyr_n.png");
  render_img_polygons(0x50, "vyr_o.png");
  render_img_polygons(0x14, "redk.png");

  render_polygons(0x15, 0xAAFFAA); // остров леса поверх вырубок

  if (draw_contours){
    filter_cnt(cnt, 0xAAFFAA); // убираем контуры, оказавшеся поверх вырубок
    draw_cnt(cnt, 0x009000, 1); // рисуем контуры
  }

  set_cap_round(); set_join_round(); set_dash(0, 2);
  render_line(0x23, 0x009000, 1, 0); // контуры, нарисованные вручную
  unset_dash();

  render_polygons(0x4d, 0xC3E6FF,20.0); // ледник

  //*******************************

  render_cnt_polygons(0x4,  0xB0B0B0, 0x000000, 0.7); // закрытые территории
  render_cnt_polygons(0xE,  0xFF8080, 0x000000, 0.7); // деревни
  render_cnt_polygons(0x1,  0xB05959, 0x000000, 0.7); // города
  render_cnt_polygons(0x4E, 0x557F55, 0x000000, 0.7); // дачи
  render_cnt_polygons(0x1A, 0x557F55, 0x000000, 0.7); // кладбища

  //*******************************

  int hor_col = 0xC06000;
  if (hr) hor_col = 0xD0B090;

  set_dash(8, 3);
  render_line(0x20, hor_col, 1, 20); // пунктирные горизонтали
  set_dash(2, 2);
  render_line(0x2B, 0xC06000, 1, 0); // сухая канава
  unset_dash();
  render_line(0x21, hor_col, 1, 20); // горизонтали
  render_line(0x22, hor_col, 1.6, 20); // жирные горизонтали

  render_line(0x25, 0xA04000, 2, 20); // овраг

  int hreb_col = 0x803000;
  if (hr) hreb_col = 0xC06000;
  render_line(0xC,  hreb_col, 2, 20); // хребет

  //*******************************

  set_cap_butt();
  render_line(0x32, 0x00B400, 3, 10); // плохой путь
  set_dash(1, 1);
  render_line(0x33, 0x00B400, 3, 10); // удовлетворительный путь
  render_line(0x34, 0xFFD800, 3, 10); // хороший путь
  unset_dash();
  render_line(0x35, 0xFFD800, 3, 10); // отличный путь

  //*******************************

  render_img_polygons(0x51, "bol_l.png"); // болота
  render_img_polygons(0x4C, "bol_h.png"); // болота труднопроходимые
  render_line(0x24, 0x5066FF, 1, 0); // старые болота

  //*******************************

  int water_col = 0x00FFFF;
  if (hr) water_col = 0x87CEFF;

  set_cap_round();
  set_dash(4, 3);
  render_line(0x26, 0x5066FF, 1, 10); // пересыхающая река
  unset_dash();
  render_line(0x15, 0x5066FF, 1, 10); // река-1
  render_line(0x18, 0x5066FF, 2, 10); // река-2
  render_line(0x1F, 0x5066FF, 3, 10); // река-3

  render_cnt_polygons(0x29, water_col, 0x5066FF, 1, 20); // водоемы
  render_cnt_polygons(0x3B, water_col, 0x5066FF, 1, 20); // большие водоемы
  render_cnt_polygons(0x53, 0xFFFFFF, 0x5066FF, 1, 20); // острова

  render_line(0x1F, water_col, 1, 10); // середина реки-3

  //*******************************

  set_cap_butt(); set_join_miter();
  render_line_el(0x1A, 0x888888, 2); // маленькая ЛЭП
  render_line_el(0x29, 0x888888, 3); // большая ЛЭП
  render_line_gaz(0x28, 0x888888, 3); // газопровод

  //*******************************

  render_line(0x5, 0, 3, 0); // дома (перенести выше?)

  set_join_round();
  set_cap_round();
  set_dash(0, 2.5);
  render_line(0x2C, hor_col, 3, 10); // вал
  unset_dash();

  //*******************************
  set_cap_butt();
  render_line(0x7, 0xFFFFFF, 3, 10); // непроезжий грейдер - белая подложка
  set_dash(5, 4); render_line(0x16, 0x0, 0.6, 0); // просека
  set_dash(8, 5); render_line(0x1C, 0x0, 1.4, 0); // просека широкая
  set_dash(6, 2); render_line(0xA,  0x0, 1, 10); // непроезжая грунтовка
  set_dash(2, 2); render_line(0x2A, 0x0, 1, 10); // тропа
  set_dash(2,1,2,3); render_line(0x2D, 0x0, 0.6, 10); // заросшая дорога
  set_dash(6, 1); render_line(0x7,  0x0, 3, 10); // непроезжий грейдер - пун
  unset_dash();
  render_line(0x6,  0x0, 1, 10); // прозжая грунтовка
  render_line(0x4,  0x0, 3, 10); // проезжий грейдер
  render_line(0x2,  0x0, 4, 10); // асфальт
  render_line(0xB,  0x0, 5, 10); // большой асфальт
  render_line(0x1,  0x0, 7, 10); // автомагистраль
  render_line(0x4,  0xFFFFFF, 1, 10); // проезжий грейдер - белая середина
  render_line(0x7,  0xFFFFFF, 1, 10); // непроезжий грейдер - белая середина
  render_line(0x2,  0xFF8080, 2, 10); // асфальт - середина
  render_line(0xB,  0xFF8080, 3, 10); // большой асфальт - середина
  render_line(0x1,  0xFF8080, 5, 10); // автомагистраль - середина
  render_line(0x1,  0x0,      1, 10); // автомагистраль - черная середина
  render_line(0xD,  0x0, 3, 10); // маленькая Ж/Д
  render_line(0x27, 0x0, 4, 10); // Ж/Д
  set_cap_round();
  set_dash(4, 2, 0, 2);   render_line(0x1D, 0x900000, 1, 0); // граница

  set_dash(2, 2); render_line(0x1E, 0x900000, 1, 0); // нижний край обрыва
  unset_dash();   render_line_obr(0x03, 0x900000, 1); // верхний край обрыва
  render_line_zab(0x19, 0x900000, 1); // забор

  render_bridge(0x1B, 0, 1, 2); // туннель
  render_bridge(0x08, 1, 1, 2); // мост-1
  render_bridge(0x09, 3, 1, 2); // мост-2
  render_bridge(0x0E, 6, 1, 2); // мост-5

  int pt_col = 0;
  if (hr) pt_col = 0x803000;

// точечные объекты
  set_cap_round();
  render_points(0x1100, pt_col, 4); // отметка высоты
  render_points(0xD00,  pt_col, 3); // маленькая отметка высоты
  render_points(0x6414, 0x5066FF, 4); // родник

  render_im_in_points(0x6402, "dom.png"); // дом
  render_im_in_points(0x1000, "ur_vod.png"); // отметка уреза воды
  render_im_in_points(0x6508, "por.png"); // порог
  render_im_in_points(0x650E, "vdp.png"); // водопад
  if (hr) render_im_in_points(0x0F00, "trig_hr.png");
  else render_im_in_points(0x0F00, "trig.png");
  render_im_in_points(0x2C04, "pam.png");
  render_im_in_points(0x2C0B, "cerkov.png");
  render_im_in_points(0x2F08, "avt.png");
  render_im_in_points(0x5905, "zd.png");
  render_im_in_points(0x6406, "per.png");
  render_im_in_points(0x6620, "pernk.png");
  render_im_in_points(0x6621, "per1a.png");
  render_im_in_points(0x6622, "per1b.png");
  render_im_in_points(0x6623, "per2a.png");
  render_im_in_points(0x6624, "per2b.png");
  render_im_in_points(0x6625, "per3a.png");
  render_im_in_points(0x6626, "per3b.png");
  render_im_in_points(0x660B, "kan.png");
  render_im_in_points(0x650A, "ldp.png");
  render_im_in_points(0x6403, "kladb.png");
  render_im_in_points(0x6411, "bash.png");
  render_im_in_points(0x6415, "razv.png");
  render_im_in_points(0x640C, "shaht.png");
  render_im_in_points(0x6603, "yama.png");
  render_im_in_points(0x6606, "ohotn.png");
  render_im_in_points(0x6613, "pupyr.png");
  render_im_in_points(0x6616, "skala.png");
  render_im_in_polygons(0x1A, "cross.png"); // крестики на кладбищах

}


void
VMAPRenderer::render_holes(){
  cr->save();
  cr->set_operator(Cairo::OPERATOR_DEST_OVER);

  bool hr = (W->style == "hr");

  // reversed order becouse of OPERATOR_DEST_OVER

/*
  render_line(0x1B, 0xFFFFFF, 1); // туннель
  render_line(0x08, 0xFFFFFF, 1); // мост-1
  render_line(0x09, 0xFFFFFF, 2); // мост-2
  render_line(0x0E, 0xFFFFFF, 5); // мост-5

  render_line(0x1,  0xFF8080, 7, 10); // автомагистраль
  render_line(0xB,  0xFF8080, 5, 10); // большой асфальт
  render_line(0x2,  0xFF8080, 4, 10); // асфальт
  render_line(0x4,  0xFFFFFF, 3, 10); // проезжий грейдер
  render_line(0x7,  0xFFFFFF, 3, 10); // непроезжий грейдер
*/

  int water_col = 0x00FFFF;
  if (hr) water_col = 0x87CEFF;

  render_line(0x1F, water_col, 3, 10); // середина реки-3
  render_polygons(0x53, 0xFFFFFF); // острова
  render_polygons(0x3B, water_col); // большие водоемы
  render_polygons(0x29, water_col); // водоемы

  int hor_col = 0xC06000;
  if (hr) hor_col = 0xD0B090;

  if (hr){
    vector<double> d;
    d.push_back(8*lw1);
    d.push_back(3*lw1);
    cr->set_dash(d, 0);
    render_line(0x20, hor_col, 1, 20); // пунктирные горизонтали
    cr->unset_dash();
    render_line(0x21, hor_col, 1, 20); // горизонтали
    render_line(0x22, hor_col, 1.6, 20); // жирные горизонтали
  }

  render_polygons(0x1A, 0xAAFFAA); // кладбища
  render_polygons(0x4E, 0xAAFFAA); // дачи
  render_polygons(0x1,  0xFFFFFF); // города
  render_polygons(0xE,  0xFFFFFF); // деревни
  render_polygons(0x4,  0xB0B0B0); // закрытые территории

  render_polygons(0x4d, 0xC3E6FF); // ледник
  render_polygons(0x15, 0xAAFFAA); // остров леса
  render_polygons(0x14, 0xFFFFFF);
  render_polygons(0x50, 0xAAFFAA);
  render_polygons(0x4f, 0xFFFFFF);
  render_polygons(0x52, 0xFFFFFF); // поле
  render_polygons(0x16, 0xAAFFAA); // лес

  cr->set_color(0xFFFFFF);
  cr->paint();

  cr->restore();
}
