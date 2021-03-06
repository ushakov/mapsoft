#include <string>
#include <sstream>
#include <iomanip>
#include <boost/lexical_cast.hpp>
#include "loaders/image_r.h"
#include "gobj_vmap.h"
#include <jansson.h>


using namespace std;



GObjVMAP::GObjVMAP(vmap::world * _W,
    const Options & O): W(_W), zc(W->style){

  pics_dpi    = O.get("pics_dpi", 600.0);
  bgcolor     = O.get("bgcolor", 0xFFFFFF);
  dpi         = O.get("dpi", 200.0);
  lw1         = O.get("line_width", dpi/105.0);
  cntrs       = O.get("contours", true);
  use_aa      = O.get("antialiasing", true);
  label_marg  = O.get("label_marg", 2.5);
  label_style = O.get("label_style", 2);
  grid_step   = O.get("grid", 0.0);
  transp      = O.get("transp_margins", false);
  grid_labels = O.get("grid_labels", 0);

  { // set pattern filter
    std::string flt = O.get<std::string>("patt_filter", "good");
    if      (flt == "fast") patt_filter = Cairo::FILTER_FAST;
    else if (flt == "good") patt_filter = Cairo::FILTER_GOOD;
    else if (flt == "best") patt_filter = Cairo::FILTER_BEST;
    else if (flt == "nearest")  patt_filter = Cairo::FILTER_NEAREST;
    else if (flt == "bilinear") patt_filter = Cairo::FILTER_BILINEAR;
    else throw Err() << "unknown patt_filter setting: " << flt;
  }

  // Read render data from json file. (2018-09)
  // File structure: json array of objects.
  // Each array element represents an action (draw object type, draw grid etc.)
  std::string render_data_fname = O.get("render_data", std::string());
  if (render_data_fname != ""){
    json_error_t e;
    json_t *J = json_load_file(render_data_fname.c_str(), 0, &e);
    if (!J) throw Err() << e.text << " in "
      << e.source << ":" << e.line << ":" << e.column;
    try {
      if (!json_is_array(J))
        throw Err() << "RenderData should be an array of objects";
      size_t i;
      json_t *c;
      json_array_foreach(J, i, c){
        Opts o;
        if (!json_is_object(c))
          throw Err() << "RenderData should be an array of objects";
        const char *k;
        json_t *v;
        json_object_foreach(c, k, v){
          if      (json_is_string(v))  o.put(k, json_string_value(v));
          else if (json_is_integer(v)) o.put(k, json_integer_value(v));
          else if (json_is_real(v))    o.put(k, json_real_value(v));
          else throw Err() << "wrong value type for " << k;
        }
        render_data.push_back(o);
      }
    }
    catch (Err e){
      json_decref(J);
      throw e;
    }
    json_decref(J);
  }
}

int
GObjVMAP::draw(iImage &img, const iPoint &org){

  if (W->brd.size()>2) ref.border=cnv.line_bck(W->brd)-org;
  if (ref.border.size()>2 &&
      rect_intersect(iRect(ref.border.range()), img.range()).empty())
        return GObj::FILL_NONE;

  // create Cairo surface and context
  cr.reset_surface(img);
  origin = org;

  if (!use_aa) cr->set_antialias(Cairo::ANTIALIAS_NONE);
  cr->set_fill_rule(Cairo::FILL_RULE_EVEN_ODD);


  // old code -- no render_data:
  if (render_data.size()==0) {
    // objects
    render_objects();

    // grid
    if (grid_step>0){
      if (ref.map_proj != Proj("tmerc"))
        cerr << "WARINIG: grid for non-tmerc maps is not supported!\n";
      render_pulk_grid(grid_step, grid_step, false, ref);
    }

    // labels
    render_labels();

    // border
    if (ref.border.size()>2)
      cr->render_border(img.range(), ref.border, transp? 0:bgcolor);

    // draw grid labels after labels
    if ((grid_step>0) && grid_labels){
      if (ref.map_proj != Proj("tmerc"))
        cerr << "WARINIG: grid for non-tmerc maps is not supported!\n";
      render_pulk_grid(grid_step, grid_step, true, ref);
    }
    return GObj::FILL_PART;
  }


  // new code
  std::vector<Opts>::const_iterator data;
  for (data=render_data.begin(); data!=render_data.end(); data++){

    // Draw points. There are two types of points: a simple point and an image.
    // { point: 0x122, image: "images/file.png"}
    // { point: 0x122, color: 0x00FF00, size: 4}
    if (data->exists("point")){
      int type = data->get<int>("point");
      int size = 4, color = 0;
      if (data->exists("image")) {
        std::string fname = data->get("image",std::string());
        iImage I = image_r::load(fname.c_str());
        if (I.empty()) throw Err() << "Can't read image: " << fname;
        auto patt = cr->img2patt(I, pics_dpi/dpi);
        if (!patt) throw Err() << "Can't create cairo pattern from image: " << fname;
        patt->set_filter(patt_filter);
        // each object, each line inside the object, each point
        for (vmap::world::const_iterator o=W->begin(); o!=W->end(); o++){
          if (o->type!=type) continue;
          for (vmap::object::const_iterator l=o->begin(); l!=o->end(); l++){
            for (dLine::const_iterator p=l->begin(); p!=l->end(); p++){
              cr->save();
              dPoint pp=(*p);
              cnv.bck(pp); pp-=origin;
              cr->translate(pp.x, pp.y);
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
      }
      else {
        cr->save();
        cr->set_line_width(lw1*data->get<int>("size", size));
        cr->set_color(data->get<int>("color", color));
        for (vmap::world::const_iterator o=W->begin(); o!=W->end(); o++){
          if (o->type!=type) continue;
          dMultiLine l = *o; cnv.line_bck_p2p(l);
          cr->mkpath_points(l-origin);
          cr->stroke();
        }
        cr->restore();
      }
    }// point

  } // for data
  return GObj::FILL_PART;
}

void
GObjVMAP::render_polygons(int type, int col, double curve_l){
  if (!cr) return;
  cr->save();
  cr->set_color(col);
  for (vmap::world::const_iterator o=W->begin(); o!=W->end(); o++){
    if (o->type!=(type | zn::area_mask)) continue;
    dMultiLine l = *o; cnv.line_bck_p2p(l);
    cr->mkpath_smline(l-origin, 1, curve_l*lw1);
    cr->fill();
  }
  cr->restore();
}

// contoured polygons
void
GObjVMAP::render_cnt_polygons(int type, int fill_col, int cnt_col,
                              double cnt_th, double curve_l){
  cr->save();
  cr->set_color(fill_col);
  for (vmap::world::const_iterator o=W->begin(); o!=W->end(); o++){
    if (o->type!= (type | zn::area_mask)) continue;
    dMultiLine l = *o; cnv.line_bck_p2p(l);
    cr->mkpath_smline(l-origin, 1, curve_l*lw1);
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
GObjVMAP::render_line(int type, int col, double th, double curve_l){
  if (!cr) return;
  cr->save();
  cr->set_line_width(th*lw1);
  cr->set_color(col);
  for (vmap::world::const_iterator o=W->begin(); o!=W->end(); o++){
    if (o->type!=(type | zn::line_mask)) continue;
    dMultiLine l = *o; cnv.line_bck_p2p(l);
    cr->mkpath_smline(l-origin, 0, curve_l*lw1);
    cr->stroke();
  }
  cr->restore();
}

void
GObjVMAP::render_points(int type, int col, double th){
  if (!cr) return;
  cr->save();
  cr->set_line_width(th*lw1);
  cr->set_color(col);
  for (vmap::world::const_iterator o=W->begin(); o!=W->end(); o++){
    if (o->type!=type) continue;
    dMultiLine l = *o; cnv.line_bck_p2p(l);
    cr->mkpath_points(l-origin);
    cr->stroke();
  }
  cr->restore();
}

void
GObjVMAP::render_img_polygons(int type, double curve_l){

  type = type | zn::area_mask;
  std::map<int, zn::zn>::const_iterator z = zc.find_type(type);
  if (z==zc.znaki.end()) return;

  string f = z->second.pic + ".png"; // picture filename
  iImage I = image_r::load(f.c_str());
  if (I.empty()) cerr << "Empty image for type " << type << "\n";

  if (pics_dpi / dpi < 12) {
    auto patt = cr->img2patt(I, pics_dpi/dpi);
    if (!patt) return;
    patt->set_filter(patt_filter);
    patt->set_extend(Cairo::EXTEND_REPEAT);
    cr->set_source(patt);
  } else {
    // Use average fill color for too little patterns on this DPI
    unsigned int r=0, g=0, b=0, a=0, alpha, c;
    for(int i = 0; i < I.h * I.w; i++) {
        c = I.data[i];
        alpha = (c >> 24) & 0xff;
        a += alpha;
        b += alpha * ((c >> 16) & 0xff);
        g += alpha * ((c >>  8) & 0xff);
        r += alpha * ((c >>  0) & 0xff);
    }
    if (a != 0) {
      b /= a;
      g /= a;
      r /= a;
    }
    a /= I.w * I.h;
    c = (a << 24) | (b << 16) | (g << 8) | r;

    cr->set_color(c);
  }

  // polygons filled with image pattern
  if (z->second.pic_type=="fill"){
    cr->save();
    for (vmap::world::const_iterator o=W->begin(); o!=W->end(); o++){
      if (o->type!=type) continue;
      dMultiLine l = *o; cnv.line_bck_p2p(l);
      cr->mkpath_smline(l-origin, 1, curve_l*lw1);
      cr->fill();
    }
    cr->restore();
  }
}

// place image in points
void
GObjVMAP::render_im_in_points(int type){

  std::map<int, zn::zn>::const_iterator z = zc.find_type(type);
  if (z==zc.znaki.end()) return;

  string f = z->second.pic + ".png"; // picture filename
  iImage I = image_r::load(f.c_str());
  if (I.empty()) cerr << "Empty image for type " << type << "\n";

  auto patt = cr->img2patt(I, pics_dpi/dpi);
  if (!patt) return;
  patt->set_filter(patt_filter);

  for (vmap::world::const_iterator o=W->begin(); o!=W->end(); o++){
    if (o->type!=type) continue;
    for (vmap::object::const_iterator l=o->begin(); l!=o->end(); l++){
      if (l->size()<1) continue;
      cr->save();
      dPoint p=(*l)[0];
      cnv.bck(p); p-=origin;
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
GObjVMAP::mkbrpath1(const vmap::object & o){
  for (vmap::object::const_iterator l=o.begin(); l!=o.end(); l++){
    if (l->size()<2) continue;
    dPoint p1 = (*l)[0], p2 = (*l)[1];
    cnv.bck(p1);  cnv.bck(p2);
    cr->move_to(p1-origin);
    cr->line_to(p2-origin);
  }
}
void
GObjVMAP::mkbrpath2(const vmap::object & o, double th, double side){
  th*=lw1/2.0;
  side*=lw1/sqrt(2.0);
  for (vmap::object::const_iterator l=o.begin(); l!=o.end(); l++){
    if (l->size()<2) continue;
    dPoint p1 = (*l)[0], p2 = (*l)[1];
    cnv.bck(p1);  cnv.bck(p2);
    p1-=origin;
    p2-=origin;
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
GObjVMAP::render_bridge(int type, double th1, double th2, double side){
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
GObjVMAP::render_line_el(Conv & cnv, int type, int col, double th, double step){
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
      LineDist ld(l1-origin);
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
GObjVMAP::render_line_obr(Conv & cnv, int type, int col, double th){
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
      LineDist ld(l1-origin);
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
GObjVMAP::render_line_zab(Conv & cnv, int type, int col, double th){
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
      LineDist ld(l1-origin);
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
GObjVMAP::render_line_val(Conv & cnv, int type, int col, double th,
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
      LineDist ld(l1-origin);
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
GObjVMAP::render_line_gaz(Conv & cnv, int type, int col, double th, double step){
  render_line(type, col, th, 0);
  double width=th*0.8*lw1;
  step*=lw1;

  cr->save();
  cr->set_line_width(lw1);
  for (vmap::world::const_iterator o=W->begin(); o!=W->end(); o++){
    if (o->type!=(type | zn::line_mask)) continue;
    for (vmap::object::const_iterator l=o->begin(); l!=o->end(); l++){
      dLine l1 = *l; cnv.line_bck_p2p(l1);
      LineDist ld(l1-origin);
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
GObjVMAP::render_grid_label(double c, double val, bool horiz, const dLine & border){

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
    cr->render_text(ss.str().c_str(), pmin, amin, 0, 18, 8, dpi, 1,ydir_min);
  if (drawmax)
    cr->render_text(ss.str().c_str(), pmax, amax, 0, 18, 8, dpi, 1,ydir_max);
}

void
GObjVMAP::render_pulk_grid(double dx, double dy, bool labels, const g_map & ref){

  convs::map2pt cnv(ref, Datum("pulkovo"), Proj("tmerc"), ref.proj_opts);

  dRect rng_m = cnv.bb_frw(ref.border.range(), 1)-origin;

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
  dPoint pbc(pb); cnv.bck(pbc); pbc-=origin;
  dPoint pec(pe); cnv.bck(pec); pec-=origin;
  // note: pb.y < pe.y, but pbc.y > pec.y!

  cr->save();
  cr->set_source_rgba(0,0,0,0.5);
  cr->set_line_width(2);
  while (p.x<pe.x){
    dPoint pc(p); cnv.bck(pc); pc-=origin;
    if (labels) render_grid_label(pc.x, p.x, true, ref.border);
    else {
      cr->Cairo::Context::move_to(pc.x, pbc.y);
      cr->Cairo::Context::line_to(pc.x, pec.y);
    }
    p.x+=dx;
  }
  while (p.y<pe.y){
    dPoint pc(p); cnv.bck(pc); pc-=origin;
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
GObjVMAP::render_objects(){

  bool hr = (W->style == "hr");

  const int c_forest = 0xAAFFAA;
  const int c_field  = 0xFFFFFF;
  const int c_fcont  = 0x009000;
  const int c_glac   = 0xFFE6C3;
  const int c_slope  = 0xCCCCCC;
  const int c_way2   = 0x00B400;
  const int c_way5   = 0x00D8FF;
  const int c_hor = hr? 0x90B0D0:0x0060C0;

  const int c_build_gray  = 0xB0B0B0;
  const int c_build_red   = 0x8080FF;
  const int c_build_dred  = 0x5959B0;
  const int c_build_green = 0x557F55;
  const int c_build_cnt   = 0x000000;

  const int c_brd         = 0x0000FF;
  const int c_brdg        = 0x00FF00;

  const int c_riv_cnt     = 0xFF6650;
  const int c_riv_fill    = hr? 0xFFCE87:0xFFFF00;
  const int c_kanav       = 0x0060C0;
  const int c_ovrag       = 0x0040A0;
  const int c_hreb        = hr? 0x0060C0:0x003080;
  const int c_lines       = 0x888888;
  const int c_road_fill   = 0x8080FF;
  const int c_obr         = 0x000090;
  const int c_pt          = hr? 0x003080:0x000000;

  //*******************************

  render_polygons(0x16, c_forest); // лес
  render_polygons(0x52, c_field);  // поле
  render_polygons(0x15, c_forest); // остров леса

  list<iPoint> cnt;
  if (cntrs) cnt = make_cnt(c_forest, 2); // контуры леса

  render_img_polygons(0x4f); // свежая вырубка
  render_img_polygons(0x50); // старая вырубка
  render_img_polygons(0x14); // редколесье
  render_polygons(0x15, c_forest); // остров леса поверх вырубок

  if (cntrs){
    filter_cnt(cnt, c_forest); // убираем контуры, оказавшеся поверх вырубок
    draw_cnt(cnt, c_fcont, 1); // рисуем контуры
  }

  cr->cap_round(); cr->join_round(); cr->set_dash(0, 2*lw1);
  render_line(0x23, c_fcont, 1, 0); // контуры, нарисованные вручную
  cr->unset_dash();

  render_polygons(0x4d, c_glac, 20.0); // ледник
  render_polygons(0x19, c_slope, 20.0); // камни, песок
  render_img_polygons(0x8); // камни, песок
  render_img_polygons(0xD); // камни, песок

  //*******************************

  // извращение с линиями проходимости:
  // сперва вырезаем место для них в подложке
  cr->save();
  cr->set_operator(Cairo::OPERATOR_CLEAR);
  cr->cap_butt();
  render_line(0x32, c_way2, 3, 10); // плохой путь
  cr->set_dash(lw1, lw1);
  render_line(0x33, c_way2, 3, 10); // удовлетворительный путь
  render_line(0x34, c_way5, 3, 10); // хороший путь
  cr->unset_dash();
  render_line(0x35, c_way2, 3, 10); // отличный путь
  cr->restore();

  //*******************************

  render_cnt_polygons(0x4,  c_build_gray,  c_build_cnt, 0.7); // закрытые территории
  render_cnt_polygons(0xE,  c_build_red,   c_build_cnt, 0.7); // деревни
  render_cnt_polygons(0x1,  c_build_dred,  c_build_cnt, 0.7); // города
  render_cnt_polygons(0x4E, c_build_green, c_build_cnt, 0.7); // дачи
  render_cnt_polygons(0x1A, c_build_green, c_build_cnt, 0.7); // кладбища

  //*******************************

  cr->set_dash(8*lw1, 3*lw1);
  render_line(0x20, c_hor, 1, 20); // пунктирные горизонтали
  cr->unset_dash();
  render_line(0x21, c_hor, 1, 20); // горизонтали
  render_line(0x22, c_hor, 1.6, 20); // жирные горизонтали

  //*******************************

  render_img_polygons(0x51); // болота
  render_img_polygons(0x4C); // болота труднопроходимые
  render_line(0x24, c_riv_cnt, 1, 0); // старые болота -- не использутся

  //*******************************


  cr->join_round();
  cr->cap_round();

  cr->set_dash(0, 2.5*lw1);
  render_line(0x2B, c_kanav, 1.6, 0); // сухая канава
  cr->unset_dash();
  render_line(0x25, c_ovrag, 2, 20); // овраг

  render_line(0xC,  c_hreb, 2,   20); // хребет
  render_line(0xF,  c_hreb, 1.5, 20); // малый хребет

  cr->set_dash(0, 2.5*lw1);
  render_line(0x2C, c_hor, 2.5, 0); // вал
  cr->unset_dash();

  //*******************************

  cr->cap_round();
  cr->set_dash(4*lw1, 3*lw1);
  render_line(0x26, c_riv_cnt, 1, 10); // пересыхающая река
  cr->unset_dash();
  render_line(0x15, c_riv_cnt, 1, 10); // река-1
  render_line(0x18, c_riv_cnt, 2, 10); // река-2
  render_line(0x1F, c_riv_cnt, 3, 10); // река-3

  render_cnt_polygons(0x29, c_riv_fill, c_riv_cnt, 1, 20); // водоемы
  render_cnt_polygons(0x3B, c_riv_fill, c_riv_cnt, 1, 20); // большие водоемы
  render_cnt_polygons(0x53, c_field, c_riv_cnt, 1, 20); // острова

  render_line(0x1F, c_riv_fill, 1, 10); // середина реки-3

  //*******************************

  // непроезжий грейдер - два ряда коричневых точек на белом фоне
  // при этом белый на самом деле вырезаем, чтоб в него попали
  // линии проходимости
  cr->save();
  cr->set_operator(Cairo::OPERATOR_CLEAR);
  render_line(0x7, c_field, 3, 0); // белое
  cr->restore();
  render_line_val(cnv, 0x7, c_hor, 1.6, 4, 2.5);
  cr->save();
  cr->set_operator(Cairo::OPERATOR_CLEAR);
  render_line(0x7, c_field, 1, 0); // белое сверху
  cr->restore();

  //*******************************

  // теперь зарисовываем то, что вырезали раньше: линии проходимости,
  // остальное - белым
  cr->save();
  cr->set_operator(Cairo::OPERATOR_DEST_OVER);
  cr->cap_butt();
  render_line(0x32, c_way2, 3, 10); // плохой путь
  cr->set_dash(lw1, lw1);
  render_line(0x33, c_way2, 3, 10); // удовлетворительный путь
  render_line(0x34, c_way5, 3, 10); // хороший путь
  cr->unset_dash();
  render_line(0x35, c_way5, 3, 10); // отличный путь
  cr->set_color(c_field);
  cr->paint();
  cr->restore();

  //*******************************

  cr->cap_butt(); cr->join_miter();
  render_line_el(cnv, 0x1A, c_lines, 2); // маленькая ЛЭП
  render_line_el(cnv, 0x29, c_lines, 3); // большая ЛЭП
  render_line_gaz(cnv, 0x28, c_lines, 3); // газопровод

  //*******************************

  cr->cap_butt();
  cr->set_dash(0.5*lw1, 1*lw1);
  render_line(0x1D, c_brd,  7, 0); // граница большая
  render_line(0x36, c_brd,  5, 0); // граница малая
  render_line(0x37, c_brdg, 5, 0); // граница зеленая

  //*******************************
  cr->cap_butt();
  cr->set_dash(5*lw1, 4*lw1); render_line(0x16, 0x0, 0.6, 0); // просека
  cr->set_dash(8*lw1, 5*lw1); render_line(0x1C, 0x0, 1.4, 0); // просека широкая
  cr->set_dash(6*lw1, 2*lw1); render_line(0xA,  0x0, 1, 10); // непроезжая грунтовка
  cr->set_dash(6*lw1, 2*lw1); render_line(0x2E,  0x0, 0.8, 10); // ММБ-грунтовка
  cr->set_dash(2*lw1, 1.5*lw1); render_line(0x2A, 0x0, 1, 10); // тропа
  cr->set_dash(2*lw1,lw1,2*lw1,3*lw1); render_line(0x2D, 0x0, 0.8, 10); // заросшая дорога
  cr->unset_dash();
  render_line(0x6,  0x0, 1, 10); // прозжая грунтовка
  render_line(0x4,  0x0, 3, 10); // проезжий грейдер
  render_line(0x2,  0x0, 4, 10); // асфальт
  render_line(0xB,  0x0, 5, 10); // большой асфальт
  render_line(0x1,  0x0, 7, 10); // автомагистраль
  render_line(0x4,  0xFFFFFF, 1, 10); // проезжий грейдер - белая середина
  render_line(0x2,  c_road_fill, 2, 10); // асфальт - середина
  render_line(0xB,  c_road_fill, 3, 10); // большой асфальт - середина
  render_line(0x1,  c_road_fill, 5, 10); // автомагистраль - середина
  render_line(0x1,  0x0,      1, 10); // автомагистраль - черная середина
  render_line(0xD,  0x0, 3, 10); // маленькая Ж/Д
  render_line(0x27, 0x0, 4, 10); // Ж/Д


  //*******************************

  cr->cap_round();
  cr->set_dash(2*lw1, 2*lw1); render_line(0x1E, c_obr, 1, 0); // нижний край обрыва
  cr->unset_dash();   render_line_obr(cnv, 0x03, c_obr, 1); // верхний край обрыва
  render_line_zab(cnv, 0x19, c_obr, 1); // забор

  render_bridge(0x1B, 0, 1, 2); // туннель
  render_bridge(0x08, 1, 1, 2); // мост-1
  render_bridge(0x09, 3, 1, 2); // мост-2
  render_bridge(0x0E, 6, 1, 2); // мост-5

  cr->cap_butt();
  render_line(0x5, 0, 3, 0); // линейные дома


// точечные объекты
  cr->cap_round();
  render_points(0x1100, c_pt, 4); // отметка высоты
  render_points(0xD00,  c_pt, 3); // маленькая отметка высоты
  render_points(0x6414, c_riv_cnt, 4); // родник
  render_points(0x660A, c_fcont,   4); // дерево

  render_im_in_points(0x6402); // дом
  render_im_in_points(0x1000); // отметка уреза воды
  render_im_in_points(0x6508); // порог
  render_im_in_points(0x650E); // водопад
  render_im_in_points(0x0F00);
  render_im_in_points(0x2C04);
  render_im_in_points(0x2C0B);
  render_im_in_points(0x2F08);
  render_im_in_points(0x5905);
  render_im_in_points(0x6406);

  render_im_in_points(0x6700); // перевалы
  render_im_in_points(0x6701);
  render_im_in_points(0x6702);
  render_im_in_points(0x6703);
  render_im_in_points(0x6704);
  render_im_in_points(0x6705);
  render_im_in_points(0x6706);

  render_im_in_points(0x660B);
  render_im_in_points(0x650A);
  render_im_in_points(0x6403);
  render_im_in_points(0x6411);
  render_im_in_points(0x6415);
  render_im_in_points(0x640C);
  render_im_in_points(0x6601); // пещера
  render_im_in_points(0x6603); // яма
  render_im_in_points(0x6606); // охотн
  render_im_in_points(0x6613); // курган
  render_im_in_points(0x6616); // скала
  render_im_in_points(0x2B03); // стоянка
  render_img_polygons(0x1A); // крестики на кладбищах

}

void
GObjVMAP::render_holes(Conv & cnv){
  cr->save();
  cr->set_operator(Cairo::OPERATOR_DEST_OVER);

  bool hr = (W->style == "hr");

  const int c_forest = 0xAAFFAA;
  const int c_field  = 0xFFFFFF;
  const int c_slope  = 0xCCCCCC;
  const int c_glac   = 0xFFE6C3;
  const int c_hor = hr? 0x90B0D0:0x0060C0;
  const int c_riv_fill    = hr? 0xFFCE87:0xFFFF00;
  const int c_road_fill   = 0x8080FF;

  // reversed order becouse of OPERATOR_DEST_OVER

  render_line(0x1F, c_riv_fill, 3, 10); // середина реки-3
  render_polygons(0x53, c_field); // острова
  render_polygons(0x3B, c_riv_fill); // большие водоемы
  render_polygons(0x29, c_riv_fill); // водоемы

  if (hr){
    vector<double> d;
    d.push_back(8*lw1);
    d.push_back(3*lw1);
    cr->set_dash(d);
    render_line(0x20, c_hor, 1, 20); // пунктирные горизонтали
    cr->unset_dash();
    render_line(0x21, c_hor, 1, 20); // горизонтали
    render_line(0x22, c_hor, 1.6, 20); // жирные горизонтали
  }

  render_polygons(0x1A, c_forest); // кладбища
  render_polygons(0x4E, c_forest); // дачи
  render_polygons(0x1,  c_field);  // города
  render_polygons(0xE,  c_field);  // деревни
  render_polygons(0x4,  c_field);  // закрытые территории

  render_polygons(0x19, c_slope);  // камни, песок
  render_polygons(0x4d, c_glac);   // ледник
  render_polygons(0x15, c_forest); // остров леса
  render_polygons(0x14, c_field);
  render_polygons(0x50, c_forest);
  render_polygons(0x4f, c_field);
  render_polygons(0x52, c_field);  // поле
  render_polygons(0x16, c_forest); // лес

  cr->set_color(bgcolor);
  cr->paint();

  cr->restore();
}

void
GObjVMAP::render_labels(){
  cr->save();

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


      for (std::list<vmap::lpos>::const_iterator
            l=o->labels.begin(); l!=o->labels.end(); l++){

        cr->set_fig_font(z->second.txt.font,
           z->second.txt.font_size + l->fsize, dpi);
        dRect ext=cr->get_text_extents(o->text);

        dPoint p(l->pos);
        cnv.bck(p); p-=origin;
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

// functions for drawing contours

// создание контура -- набор точек на расстоянии dist друг от друга
// вокруг областей с цветом col. Цвет в точках = col
std::list<iPoint>
GObjVMAP::make_cnt(int col, double dist){
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
GObjVMAP::filter_cnt(std::list<iPoint> & cnt, int col){
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
GObjVMAP::draw_cnt(const std::list<iPoint> & cnt, int c, double th){
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
