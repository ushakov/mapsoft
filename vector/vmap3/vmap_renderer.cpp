#include "vmap_renderer.h"
#include <string>
#include <sstream>
#include <iomanip>
#include <boost/lexical_cast.hpp>
#include "loaders/image_r.h"

using namespace std;

VMAPRenderer::VMAPRenderer(vmap::world * _W, iImage & img,
    const Options & O): W(_W) {

  pics_dpi    = O.get("pics_dpi", 600.0);
  pics_dir    = O.get("pics_dir", string("/usr/share/mapsoft/pics"));
  bgcolor     = O.get("bgcolor", 0xFFFFFF);
  dpi         = O.get("dpi", 300.0);
  lw1         = O.get("line_width", dpi/105.0);
  cntrs       = O.get("contours", true);
  use_aa      = O.get("antialiasing", true);
  label_marg  = O.get("label_marg", 2.5);
  label_style = O.get("label_style", 2);

  // create Cairo surface and context
  cr.reset_surface(img);

  // antialiasing is not compatable with erasing
  // dark points under labels
  if (!use_aa) cr->set_antialias(Cairo::ANTIALIAS_NONE); 
}

void
render_border(iImage & img, const dLine & brd, const Options & O){
  CairoWrapper cr(img);

  int bgcolor = O.get<int>("bgcolor", 0xFFFFFF);
  bool transp = O.get<bool>("transp_margins", false);

  // make border path
  dLine::const_iterator p;
  cr->set_fill_rule(Cairo::FILL_RULE_EVEN_ODD);
  cr->mkpath(brd);
  cr->mkpath(rect2line(img.range()));

  // erase everything outside border
  if (transp) cr->set_operator(Cairo::OPERATOR_CLEAR);
  else  cr->set_color(bgcolor);
  cr->fill_preserve();

  // draw border
  cr->set_source_rgb(0,0,0);
  cr->set_line_width(2);
  cr->stroke();
}

void
VMAPRenderer::render_polygons(int type, int col, double curve_l){
  if (!cr) return;
  cr->save();
  cr->set_color(col);
  for (vmap::world::const_iterator o=W->begin(); o!=W->end(); o++){
    if (o->type!=(type | zn::area_mask)) continue;
    dMultiLine l = *o; cnv.line_bck_p2p(l);
    cr->mkpath_smline(l, 1, curve_l*lw1);
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
    dMultiLine l = *o; cnv.line_bck_p2p(l);
    cr->mkpath_smline(l, 1, curve_l*lw1);
    cr->fill_preserve();
    cr->save();
    cr->set_color(cnt_col);
    cr->set_line_width(cnt_th*lw1);
    cr->stroke();
    cr->restore();
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
    dMultiLine l = *o; cnv.line_bck_p2p(l);
    cr->mkpath_smline(l, 0, curve_l*lw1);
    cr->stroke();
  }
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
    dMultiLine l = *o; cnv.line_bck_p2p(l);
    cr->mkpath_points(l);
    cr->stroke();
  }
  cr->restore();
}

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
      cnv.bck(p);
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
    dMultiLine l = *o; cnv.line_bck_p2p(l);
    cr->mkpath_smline(l, 1, curve_l*lw1);
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
      dPoint p=(*l)[0];
      cnv.bck(p);
      cr->translate(p.x, p.y);
      if (o->opts.exists("Angle")){
        double a = o->opts.get<double>("Angle",0);
        a = cnv.ang_bck(o->center(), M_PI/180 * a, 0.01);
        cr->rotate(a);
      }
      cr->set_source(patt);
      cr->paint();
      cr->restore();
    }
  }
}



// paths for bridge sign
void
VMAPRenderer::mkbrpath1(const vmap::object & o){
  for (vmap::object::const_iterator l=o.begin(); l!=o.end(); l++){
    if (l->size()<2) continue;
    dPoint p1 = (*l)[0], p2 = (*l)[1];
    cnv.bck(p1);  cnv.bck(p2);
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
    cnv.bck(p1);  cnv.bck(p2);
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
      cr->stroke();
    }
    th1+=th2;
  }
  cr->set_line_width(th2*lw1);
  cr->set_color(0x0);
  for (vmap::world::const_iterator o=W->begin(); o!=W->end(); o++){
    if (o->type!=(type | zn::line_mask)) continue;
    mkbrpath2(*o, th1, side);
    cr->stroke();
  }
  cr->restore();
}


void
VMAPRenderer::render_line_el(Conv & cnv, int type, int col, double th, double step){
  render_line(type, col, th, 0);
  double width=th*1.2*lw1;
  step*=lw1;

  cr->save();
  cr->cap_round();
  cr->set_line_width(1.8*lw1);
  cr->set_color(col);
  for (vmap::world::const_iterator o=W->begin(); o!=W->end(); o++){
    if (o->type!=(type | zn::line_mask)) continue;
    for (vmap::object::const_iterator l=o->begin(); l!=o->end(); l++){
      dLine l1 = *l; cnv.line_bck_p2p(l1);
      LineDist ld(l1);
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
    cr->stroke();
  }
  cr->restore();
}

void
VMAPRenderer::render_line_obr(Conv & cnv, int type, int col, double th){
  render_line(type, col, th, 0);
  double width=th*2*lw1;
  double step=th*4*lw1;

  cr->save();
  cr->cap_round();
  cr->set_line_width(th*lw1);
  cr->set_color(col);

  for (vmap::world::const_iterator o=W->begin(); o!=W->end(); o++){
    if (o->type!=(type | zn::line_mask)) continue;
    int k=1;
    if (o->dir==2) k=-1;
    for (vmap::object::const_iterator l=o->begin(); l!=o->end(); l++){
      dLine l1 = *l; cnv.line_bck_p2p(l1);
      LineDist ld(l1);
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
    cr->stroke();
  }
  cr->restore();
}

void
VMAPRenderer::render_line_zab(Conv & cnv, int type, int col, double th){
  render_line(type, col, th, 0);
  double width=th*2*lw1;
  double step=th*8*lw1;

  cr->save();
  cr->cap_round();
  cr->set_line_width(th*lw1);
  cr->set_color(col);

  for (vmap::world::const_iterator o=W->begin(); o!=W->end(); o++){
    if (o->type!=(type | zn::line_mask)) continue;
    int k=1;
    if (o->dir==2) k=-1;
    for (vmap::object::const_iterator l=o->begin(); l!=o->end(); l++){
      dLine l1 = *l; cnv.line_bck_p2p(l1);
      LineDist ld(l1);
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
    cr->stroke();
  }
  cr->restore();
}

void
VMAPRenderer::render_line_val(Conv & cnv, int type, int col, double th,
                              double width, double step){
  width*=lw1/2;
  step*=lw1;

  cr->save();
  cr->cap_round();
  cr->set_line_width(th*lw1);
  cr->set_color(col);

  for (vmap::world::const_iterator o=W->begin(); o!=W->end(); o++){
    if (o->type!=(type | zn::line_mask)) continue;
    for (vmap::object::const_iterator l=o->begin(); l!=o->end(); l++){
      dLine l1 = *l; cnv.line_bck_p2p(l1);
      LineDist ld(l1);
      if (ld.length()<=step) continue;
      double fstep = ld.length()/ceil(ld.length()/step);
      int n=0;
      while (ld.dist() < ld.length()){
       dPoint p=ld.pt(), v=ld.norm()*width, t=ld.tang()*step/2;
        cr->move_to(p+v);
        cr->line_to(p+v);
        cr->move_to(p-v);
        cr->line_to(p-v);
//        cr->move_to(p-v);
//        cr->line_to(p+v);
        ld.move_frw(fstep);
        n++;
      }
    }
    cr->stroke();
  }
  cr->restore();
}

void
VMAPRenderer::render_line_gaz(Conv & cnv, int type, int col, double th, double step){
  render_line(type, col, th, 0);
  double width=th*0.8*lw1;
  step*=lw1;

  cr->save();
  cr->set_line_width(lw1);
  for (vmap::world::const_iterator o=W->begin(); o!=W->end(); o++){
    if (o->type!=(type | zn::line_mask)) continue;
    for (vmap::object::const_iterator l=o->begin(); l!=o->end(); l++){
      dLine l1 = *l; cnv.line_bck_p2p(l1);
      LineDist ld(l1);
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
VMAPRenderer::render_grid_label(double c, double val, bool horiz, const dLine & border){

  ostringstream ss;
  ss << setprecision(7) << val/1000.0;

  if (border.size()<1) return;

  dPoint pmin, pmax;
  double amin=0, amax=0;
  dLine::const_iterator i,j;
  for (i=border.begin(); i!=border.end(); i++){
    j=i+1;
    if (j==border.end()) j=border.begin();
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
VMAPRenderer::render_pulk_grid(double dx, double dy, bool labels, const g_map & ref){

  convs::map2pt cnv(ref, Datum("pulkovo"), Proj("tmerc"), ref.proj_opts);

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
    if (labels) render_grid_label(pc.x, p.x, true, ref.border);
    else {
      cr->Cairo::Context::move_to(pc.x, pbc.y);
      cr->Cairo::Context::line_to(pc.x, pec.y);
    }
    p.x+=dx;
  }
  while (p.y<pe.y){
    dPoint pc(p); cnv.bck(pc);
    if (labels && (p.y > pb.y+m) && (p.y<pe.y-m))
      render_grid_label(pc.y, p.y, false, ref.border);
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

  cr->cap_round(); cr->join_round(); cr->set_dash(0, 2*lw1);
  render_line(0x23, 0x009000, 1, 0); // контуры, нарисованные вручную
  cr->unset_dash();

  render_polygons(0x4d, 0xC3E6FF,20.0); // ледник

  //*******************************

  // извращение с линиями проходимости:
  // сперва вырезаем место для них в подложке
  cr->save();
  cr->set_operator(Cairo::OPERATOR_CLEAR);
  cr->cap_butt();
  render_line(0x32, 0x00B400, 3, 10); // плохой путь
  cr->set_dash(lw1, lw1);
  render_line(0x33, 0x00B400, 3, 10); // удовлетворительный путь
  render_line(0x34, 0xFFD800, 3, 10); // хороший путь
  cr->unset_dash();
  render_line(0x35, 0xFFD800, 3, 10); // отличный путь
  cr->restore();

  //*******************************

  render_cnt_polygons(0x4,  0xB0B0B0, 0x000000, 0.7); // закрытые территории
  render_cnt_polygons(0xE,  0xFF8080, 0x000000, 0.7); // деревни
  render_cnt_polygons(0x1,  0xB05959, 0x000000, 0.7); // города
  render_cnt_polygons(0x4E, 0x557F55, 0x000000, 0.7); // дачи
  render_cnt_polygons(0x1A, 0x557F55, 0x000000, 0.7); // кладбища

  //*******************************

  int hor_col = 0xC06000;
  if (hr) hor_col = 0xD0B090;

  cr->set_dash(8*lw1, 3*lw1);
  render_line(0x20, hor_col, 1, 20); // пунктирные горизонтали
  cr->unset_dash();
  render_line(0x21, hor_col, 1, 20); // горизонтали
  render_line(0x22, hor_col, 1.6, 20); // жирные горизонтали

  //*******************************

  render_img_polygons(0x51, "bol_l.png"); // болота
  render_img_polygons(0x4C, "bol_h.png"); // болота труднопроходимые
  render_line(0x24, 0x5066FF, 1, 0); // старые болота

  //*******************************

  cr->join_round();
  cr->cap_round();

  cr->set_dash(0, 2.5*lw1);
  render_line(0x2B, 0xC06000, 1.6, 0); // сухая канава
  cr->unset_dash();
  render_line(0x25, 0xA04000, 2, 20); // овраг

  int hreb_col = 0x803000;
  if (hr) hreb_col = 0xC06000;
  render_line(0xC,  hreb_col, 2, 20); // хребет

  cr->set_dash(0, 2.5*lw1);
  render_line(0x2C, hor_col, 2.5, 0); // вал
  cr->unset_dash();

  //*******************************

  int water_col = 0x00FFFF;
  if (hr) water_col = 0x87CEFF;

  cr->cap_round();
  cr->set_dash(4*lw1, 3*lw1);
  render_line(0x26, 0x5066FF, 1, 10); // пересыхающая река
  cr->unset_dash();
  render_line(0x15, 0x5066FF, 1, 10); // река-1
  render_line(0x18, 0x5066FF, 2, 10); // река-2
  render_line(0x1F, 0x5066FF, 3, 10); // река-3

  render_cnt_polygons(0x29, water_col, 0x5066FF, 1, 20); // водоемы
  render_cnt_polygons(0x3B, water_col, 0x5066FF, 1, 20); // большие водоемы
  render_cnt_polygons(0x53, 0xFFFFFF, 0x5066FF, 1, 20); // острова

  render_line(0x1F, water_col, 1, 10); // середина реки-3

  //*******************************

  // непроезжий грейдер - два ряда коричневых точек на белом фоне
  // при этом белый на самом деле вырезаем, чтоб в него попали
  // линии проходимости
  cr->save();
  cr->set_operator(Cairo::OPERATOR_CLEAR);
  render_line(0x7, 0xFFFFFF, 3, 0); // белое
  cr->restore();

  render_line_val(cnv, 0x7, hor_col, 1.6, 4, 2.5);

  cr->save();
  cr->set_operator(Cairo::OPERATOR_CLEAR);
  render_line(0x7, 0xFFFFFF, 1, 0); // белое сверху
  cr->restore();

  //*******************************

  // теперь зарисовываем то, что вырезали раньше: линии проходимости,
  // остальное - белым
  cr->save();
  cr->set_operator(Cairo::OPERATOR_DEST_OVER);
  cr->cap_butt();
  render_line(0x32, 0x00B400, 3, 10); // плохой путь
  cr->set_dash(lw1, lw1);
  render_line(0x33, 0x00B400, 3, 10); // удовлетворительный путь
  render_line(0x34, 0xFFD800, 3, 10); // хороший путь
  cr->unset_dash();
  render_line(0x35, 0xFFD800, 3, 10); // отличный путь
  cr->set_color(0xFFFFFF);
  cr->paint();
  cr->restore();

  //*******************************

  cr->cap_butt(); cr->join_miter();
  render_line_el(cnv, 0x1A, 0x888888, 2); // маленькая ЛЭП
  render_line_el(cnv, 0x29, 0x888888, 3); // большая ЛЭП
  render_line_gaz(cnv, 0x28, 0x888888, 3); // газопровод

  //*******************************
  cr->cap_butt();
  cr->set_dash(5*lw1, 4*lw1); render_line(0x16, 0x0, 0.6, 0); // просека
  cr->set_dash(8*lw1, 5*lw1); render_line(0x1C, 0x0, 1.4, 0); // просека широкая
  cr->set_dash(6*lw1, 2*lw1); render_line(0xA,  0x0, 1, 10); // непроезжая грунтовка
  cr->set_dash(2*lw1, 1.5*lw1); render_line(0x2A, 0x0, 1, 10); // тропа
  cr->set_dash(2*lw1,lw1,2*lw1,3*lw1); render_line(0x2D, 0x0, 0.8, 10); // заросшая дорога
  cr->unset_dash();
  render_line(0x6,  0x0, 1, 10); // прозжая грунтовка
  render_line(0x4,  0x0, 3, 10); // проезжий грейдер
  render_line(0x2,  0x0, 4, 10); // асфальт
  render_line(0xB,  0x0, 5, 10); // большой асфальт
  render_line(0x1,  0x0, 7, 10); // автомагистраль
  render_line(0x4,  0xFFFFFF, 1, 10); // проезжий грейдер - белая середина
  render_line(0x2,  0xFF8080, 2, 10); // асфальт - середина
  render_line(0xB,  0xFF8080, 3, 10); // большой асфальт - середина
  render_line(0x1,  0xFF8080, 5, 10); // автомагистраль - середина
  render_line(0x1,  0x0,      1, 10); // автомагистраль - черная середина
  render_line(0xD,  0x0, 3, 10); // маленькая Ж/Д
  render_line(0x27, 0x0, 4, 10); // Ж/Д
  cr->cap_round();
  cr->set_dash(4*lw1, 2*lw1, 0, 2*lw1);   render_line(0x1D, 0x900000, 1, 0); // граница

  cr->set_dash(2*lw1, 2*lw1); render_line(0x1E, 0x900000, 1, 0); // нижний край обрыва
  cr->unset_dash();   render_line_obr(cnv, 0x03, 0x900000, 1); // верхний край обрыва
  render_line_zab(cnv, 0x19, 0x900000, 1); // забор

  render_bridge(0x1B, 0, 1, 2); // туннель
  render_bridge(0x08, 1, 1, 2); // мост-1
  render_bridge(0x09, 3, 1, 2); // мост-2
  render_bridge(0x0E, 6, 1, 2); // мост-5

  cr->cap_butt();
  render_line(0x5, 0, 3, 0); // линейные дома

  int pt_col = 0;
  if (hr) pt_col = 0x803000;

// точечные объекты
  cr->cap_round();
  render_points(0x1100, pt_col, 4); // отметка высоты
  render_points(0xD00,  pt_col, 3); // маленькая отметка высоты
  render_points(0x6414, 0x5066FF, 4); // родник

  render_im_in_points(0x6402, "dom.png"); // дом
  render_im_in_points(0x1000, "ur_vod.png"); // отметка уреза воды
  render_im_in_points(0x6508, "por.png"); // порог
  render_im_in_points(0x650E, "vdp.png"); // водопад
  render_im_in_points(0x0F00, hr?"trig_hr.png":"trig.png");
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
VMAPRenderer::render_holes(Conv & cnv){
  cr->save();
  cr->set_operator(Cairo::OPERATOR_DEST_OVER);

  bool hr = (W->style == "hr");

  // reversed order becouse of OPERATOR_DEST_OVER

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
    cr->set_dash(d);
    render_line(0x20, hor_col, 1, 20); // пунктирные горизонтали
    cr->unset_dash();
    render_line(0x21, hor_col, 1, 20); // горизонтали
    render_line(0x22, hor_col, 1.6, 20); // жирные горизонтали
  }

  render_polygons(0x1A, 0xAAFFAA); // кладбища
  render_polygons(0x4E, 0xAAFFAA); // дачи
  render_polygons(0x1,  0xFFFFFF); // города
  render_polygons(0xE,  0xFFFFFF); // деревни
  render_polygons(0x4,  0xFFFFFF); // закрытые территории

  render_polygons(0x4d, 0xC3E6FF); // ледник
  render_polygons(0x15, 0xAAFFAA); // остров леса
  render_polygons(0x14, 0xFFFFFF);
  render_polygons(0x50, 0xAAFFAA);
  render_polygons(0x4f, 0xFFFFFF);
  render_polygons(0x52, 0xFFFFFF); // поле
  render_polygons(0x16, 0xAAFFAA); // лес

  cr->set_color(bgcolor);
  cr->paint();

  cr->restore();
}

void
VMAPRenderer::render_labels(){
  cr->save();

  zn::zn_conv zc(W->style);

  for (int pass=1; pass<=2; pass++){

    // no need for first pass
    if ((pass==1) && (label_style==LABEL_STYLE0)) continue;

    for (vmap::world::const_iterator o=W->begin(); o!=W->end(); o++){
      std::map<int, zn::zn>::const_iterator z = zc.find_type(o->type);
      if (z==zc.znaki.end()) continue;
      if (!z->second.label_type) continue;

      if (pass==1){
        cr->set_line_width(label_marg*lw1);
        cr->set_line_join(Cairo::LINE_JOIN_ROUND);
        if (label_style==LABEL_STYLE1){ // lighten everything below text
          cr->set_source_rgba(1,1,1,0.5);
        }
        else if (label_style==LABEL_STYLE2){ // erase everything below text
          cr->set_operator(Cairo::OPERATOR_SOURCE);
          cr->set_source_rgba(1,1,1,0);
        }
      }
      else { // pass 2 -- draw text
        cr->set_color(z->second.txt.pen_color);
      }

      cr->set_fig_font(z->second.txt.font, z->second.txt.font_size, dpi);

      dRect ext=cr->get_text_extents(o->text);

      for (std::list<vmap::lpos>::const_iterator
            l=o->labels.begin(); l!=o->labels.end(); l++){
        dPoint p(l->pos);
        cnv.bck(p);
        cr->save();
        cr->move_to(p);
        double a = l->ang;
        a = cnv.ang_bck(o->center(), M_PI/180 * a, 0.01);
        if (!l->hor) cr->rotate(a);
        if (l->dir == 1) cr->Cairo::Context::rel_move_to(-ext.w/2, 0);
        if (l->dir == 2) cr->Cairo::Context::rel_move_to(-ext.w, 0);
        if (pass == 1){
          cr->text_path(o->text);
          cr->stroke();
        }
        else {
          cr->show_text(o->text); // draw text
        }
        cr->restore();
      }
    }
  }
  cr->restore();
  if (label_style==LABEL_STYLE2) render_holes(cnv);
}

void
VMAPRenderer::render_text(const char *text, dPoint pos, double ang,
       int color, int fig_font, int font_size, int hdir, int vdir){
  cr->save();
  cr->set_color(color);
  cr->set_fig_font(fig_font, font_size, dpi);

  dRect ext = cr->get_text_extents(text);

  if (pos.x<0) pos.x=cr.get_im_surface()->get_width()+pos.x;
  if (pos.y<0) pos.y=cr.get_im_surface()->get_height()+pos.y;
  cr->move_to(pos);
  cr->rotate(ang);
  if (hdir == 1) cr->Cairo::Context::rel_move_to(-ext.w/2, 0.0);
  if (hdir == 2) cr->Cairo::Context::rel_move_to(-ext.w, 0.0);
  if (vdir == 1) cr->Cairo::Context::rel_move_to(0.0, ext.h/2);
  if (vdir == 2) cr->Cairo::Context::rel_move_to(0.0, ext.h);

  cr->reset_clip();

  cr->show_text(text);
  cr->restore();
}

// functions for drawing contours

// создание контура -- набор точек на расстоянии dist друг от друга
// вокруг областей с цветом col. Цвет в точках = col
std::list<iPoint>
VMAPRenderer::make_cnt(int col, double dist){
  dist*=lw1;

#define COL(x,y)  ((data[s*(y) + 4*(x) + 2] << 16)\
               + (data[s*(y) + 4*(x) + 1] << 8)\
               +  data[s*(y) + 4*(x) + 0])
#define PCOL(p)  COL((p).x,(p).y)
#define ADJ(p,i)  COL( (p).x + ((i==1)?1:0) - ((i==3)?1:0),\
                       (p).y + ((i==2)?1:0) - ((i==0)?1:0))

  // data and s indentifires used in COL macro!
  unsigned char *data=cr.get_im_surface()->get_data();
  int w=cr.get_im_surface()->get_width();
  int h=cr.get_im_surface()->get_height();
  int s=cr.get_im_surface()->get_stride();
  std::list<iPoint> points;
  std::list<iPoint> ret;

  // walk through all non-border points
  iPoint p;
  for (p.y=1; p.y<h-1; p.y++){
    for (p.x=1; p.x<w-1; p.x++){
      bool mk_pt=false;
      if (PCOL(p) != col) continue;
      for (int k=0;k<4;k++){
        if (ADJ(p,k) != col) {
          mk_pt=true;
          break;
        }
      }
      if (!mk_pt) continue;


      std::list<iPoint>::iterator i=points.begin();
      while (i!=points.end()){
        if (pdist(p - *i) < dist) {
          mk_pt=false;
          break;
        }
	// We scan the field from top to bottom. Therefore we will
	// never use points that are higher than (current y - dist)
	// again.
        if (abs(p.y - i->y) > dist) {
          i=points.erase(i);
        }
        else {
          i++;
        }
      }
      if (!mk_pt) continue;
      points.push_back(p);
      ret.push_back(p);
    }
  }
  return ret;
}

void
VMAPRenderer::filter_cnt(std::list<iPoint> & cnt, int col){
  // data and s indentifires used in COL macro!
  unsigned char *data=cr.get_im_surface()->get_data();
  int w=cr.get_im_surface()->get_width();
  int h=cr.get_im_surface()->get_height();
  int s=cr.get_im_surface()->get_stride();
  std::list<iPoint>::iterator p=cnt.begin();
  while (p != cnt.end()){
    if (p->x >= w || p->x < 0 || p->y >= h || p->y < 0) {
      p = cnt.erase(p);
    } else if (PCOL(*p)!=col) {
      p = cnt.erase(p);
    } else {
      p++;
    }
  }
}

// рисование уже готового контура
void
VMAPRenderer::draw_cnt(const std::list<iPoint> & cnt, int c, double th){
  cr->save();
  cr->begin_new_path();
  cr->set_line_width(th*lw1);
  cr->set_color(c);
  cr->set_line_cap(Cairo::LINE_CAP_ROUND);

  for ( std::list<iPoint>::const_iterator p=cnt.begin(); p!=cnt.end(); p++){
    cr->move_to(*p);
    cr->rel_line_to(dPoint());
  }
  cr->stroke();
  cr->restore();
}
