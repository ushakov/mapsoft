#include <list>
#include <vector>
#include <string>
#include <map>
#include <iostream>

#include "lib2d/line_utils.h"
#include "libgeo_io/geofig.h"
#include "libgeo/geo_convs.h"
#include "../libzn/zn.h"
#include "vmap.h"

namespace vmap {

using namespace std;

const int label_type = 0x1000FF;
const int border_type = 0x1000FE;

const double label_search_dist1 = 1.0; // cm
const double label_search_dist2 = 1.0;
const double label_search_dist3 = 0.1;
const double label_len          = 0.3;
const double label_new_dist     = 0.1;

const double default_rscale = 50000;
const string default_style = "default";

/***************************************/

dRect
world::range() const{
  if (this->size()<1) return dRect(0,0,0,0);
  world::const_iterator i=this->begin();
  dRect ret=i->range();
  while ((++i) != this->end())
    ret = rect_bounding_box(ret, i->range());
  return ret;
}


/*
 Процедура нахождения соответствия между подписями и объектами:
 1. Каждый объект берет по одной подписи с правильным текстом и с точкой
    привязки не далее label_search_dist1 от него.
 2. Каждый объект берет все остальные подписи с правильным текстом и с
    точкой привязки или положением не далее label_search_dist2 от него.
 3. Каждый объект берет все подписи, с точкой привязки не далее
    label_search_dist3 от него.
 4. Невостребованные подписи теряются.

  Считается что label_search_dist2 существенно больше label_search_dist3, так что
  для того, чтобы не потерять подпись, надо либо двигать, либо
  переименовывать объект, но не делать это одновременно.
*/
int
world::add_labels(world & objects,  std::list<lpos_full> & labels){
  std::list<lpos_full>::iterator l, l0;
  world::iterator o;
  dPoint p, p0;

  // one nearest label with correct text (label_search_dist1 parameter)
  for (o=objects.begin(); o!=objects.end(); o++){
    if (o->text == "") continue;
    double d0=1e99;
    for (l=labels.begin(); l!=labels.end(); l++){
      if (l->text != o->text) continue;
      double d = dist_pt_l(l->ref, *o, p);
      if ( d < d0){ d0=d; p0=p; l0=l;}
    }
    if (d0 * 100 / rscale < label_search_dist1){
      o->labels.push_back(*l0);
      labels.erase(l0);
    }
  }

  // labels with correct text (label_search_dist2 parameter)
  for (o=objects.begin(); o!=objects.end(); o++){
    if (o->text == "") continue;
    l=labels.begin();
    while (l!=labels.end()){
      // near ref or near pos
      if ( ((l->text == o->text) &&
            (dist_pt_l(l->ref, *o, p)* 100 / rscale < label_search_dist2)) ||
           ((l->text == o->text) &&
            (dist_pt_l(l->pos, *o, p)* 100 / rscale < label_search_dist2)) ){
        o->labels.push_back(*l);
        l = labels.erase(l);
        continue;
      }
      l++;
    }
  }

  // labels with changed text (label_search_dist3 parameter)
  for (o=objects.begin(); o!=objects.end(); o++){
    if (o->text == "") continue;
    l=labels.begin();
    while (l!=labels.end()){
      if (dist_pt_l(l->ref, *o, p)* 100 / rscale < label_search_dist3){
        o->labels.push_back(*l);
        l = labels.erase(l);
        continue;
      }
      l++;
    }
  }
}

/***************************************/

dPoint
max_xmy(const dLine & l){
  if (l.size()<1) return dPoint(0,0);
  dPoint p(l[0]);
  double max = p.x-p.y;
  for (dLine::const_iterator i = l.begin(); i!=l.end(); i++){
    if (i->x - i->y > max) {
      max = i->x - i->y;
      p = *i;
    }
  }
  return p;
}
dPoint
max_xpy(const dLine & l){
  if (l.size()<1) return dPoint(0,0);
  dPoint p(l[0]);
  double max = p.x+p.y;
  for (dLine::const_iterator i = l.begin(); i!=l.end(); i++){
    if (i->x + i->y > max) {
      max = i->x + i->y;
      p = *i;
    }
  }
  return p;
}

// create new labels
void
world::new_labels(world & objects, zn::zn_conv & zconverter){
  for (world::iterator o=objects.begin(); o!=objects.end(); o++){
    if ((o->labels.size()>0) || (o->text=="")) continue;

    std::map<int, zn::zn>::const_iterator z = zconverter.znaki.find(o->type);
    if (z==zconverter.znaki.end()) continue;
    int lp = z->second.label_pos;
    if (!lp) continue; // no label for this object

    for (dMultiLine::iterator i=o->begin(); i!=o->end(); i++){

      if (i->size()<1) continue;
      lpos l;
      l.ang = 0;
      l.dir = z->second.label_dir;
      switch (lp){
        case 1: // TR side of object (max x-y point + (1,-1)*td)
          l.pos = max_xpy(*i) +
            v_m2deg(rscale / 100.0 * dPoint(1,1) * label_new_dist/sqrt(2.0), (*i)[0].y);
          break;
        case 2: // TL side of object
          l.pos = max_xmy(*i) +
            v_m2deg(rscale / 100.0 * dPoint(-1,1) * label_new_dist/sqrt(2.0), (*i)[0].y);
          break;
        case 3: // object center
          l.pos = i->range().CNT();
          break;
        case 4: // along the line
          if (i->size()>=2){
            dPoint cp = ((*i)[i->size()/2-1] + (*i)[i->size()/2]) / 2;
            dPoint dp =  (*i)[i->size()/2-1] - (*i)[i->size()/2];
            if ((dp.x == 0) && (dp.y == 0)) dp.x = 1;
            dPoint v = pnorm(dp);
            if (v.x<0) v=-1.0*v;
            l.ang = atan2(v.y, v.x);
            l.pos = cp +
              v_m2deg(rscale / 100.0 * dPoint(-v.y, v.x)*label_new_dist, (*i)[0].y);
          }
          else if (i->size()==1) l.pos = (*i)[0];
          break;
      }
      o->labels.push_back(l);
    }
  }
}

// input angle conversions:
//                           ll p1,p2 -> ll angle   : mp labels
//              fig p1,p2 -> ll p1,p2 -> ll angle   : fig object labels
// fig angle -> fig p1,p2 -> ll p1,p2 -> ll angle   : fig text labels
double
world::ang_pll2a(const dPoint & p1, const dPoint & p2, int dir){
  dPoint v = (dir == 2)? p1-p2 : p2-p1;
  if (dir == 2) v=-1.0*v;
  return atan2(v.y, v.x);
}
double
world::ang_pfig2a(const dPoint & p1, const dPoint & p2, int dir, Conv & cnv){
  dPoint p1l(p1), p2l(p2);
  cnv.frw(p1l); cnv.frw(p2l);
  return ang_pll2a(p1l, p2l, dir);
}
double
world::ang_afig2a(double afig, int dir, Conv & cnv, dPoint fig_pos){
  double llen = label_len * fig::cm2fig;
  dPoint dp = llen * dPoint(cos(afig), -sin(afig));
  return ang_pfig2a(fig_pos, fig_pos+dp, dir, cnv);
}
// output angle conversions
// ll angle -> ll dp                                : mp labels
// ll angle -> ll dp -> fig dp                      : fig object labels
// ll angle -> ll dp -> fig dp -> fig a             : fig text labels
dPoint
world::ang_a2pll(double a, int dir, dPoint pos){
  dPoint v(cos(a), sin(a));
  v*=pdist(v_m2deg(rscale / 100.0 * label_len * v, pos.y));
  v*=(dir == 2)? -1.0:1.0;
  return pos+v;
}
dPoint
world::ang_a2pfig(double a, int dir, Conv & cnv, dPoint fig_pos){
  dPoint pos(fig_pos); cnv.frw(pos);
  dPoint ret = ang_a2pll(a, dir, pos);
  cnv.bck(ret);
  return ret;
}
double
world::ang_a2afig(double a, Conv & cnv, dPoint fig_pos){
  dPoint v = ang_a2pfig(a, 0, cnv, fig_pos) - fig_pos;
  return atan2(-v.y, v.x);
}


/***************************************/

// get vmap objects and labels from fig
int
world::get(const fig::fig_world & F){
  // get fig reference
  g_map ref = fig::get_ref(F);
  if (ref.size()<3){
    std::cerr << "ERR: not a GEO-fig\n"; return 0;
  }
  convs::map2pt cnv(ref, Datum("wgs84"), Proj("lonlat"));

  // get map data
  rscale = 100 * convs::map_mpp(ref, Proj("tmerc")) * fig::cm2fig;
  rscale = F.opts.get<double>("rscale", rscale);
  style  = F.opts.get<string>("style", default_style);
  name   = F.opts.get<string>("name");
  mp_id  = F.opts.get<int>("mp_id");

  // get map objects and labels:
  zn::zn_conv zconverter(style);
  std::list<lpos_full> labels; // ownerless labels
  world o4l; // objects waiting for a label
  std::vector<std::string> cmp_comm, comm;
  for (fig::fig_world::const_iterator i=F.begin(); i!=F.end(); i++){
    if (i->type==6)  cmp_comm = i->comment;
    if (i->type==-6) cmp_comm.clear();
    if (zn::is_to_skip(*i)) continue;

    // read map objects
    int type=0;
    if (zconverter.is_map_depth(*i) &&
       (type = zconverter.get_type(*i)) ){

      if (cmp_comm.size()>0) comm=cmp_comm;
      else comm=i->comment;

      if (type==border_type){ // special type -- border
        brd = cnv.line_frw(*i);
        continue;
      }
      if (type==label_type){ // special type -- label objects
        if (i->size()<2) continue;
        if (i->comment.size()<1) continue;
        lpos_full l;
        l.text = i->comment[0];
        l.ref = (*i)[0]; cnv.frw(l.ref);
        l.pos = (*i)[1]; cnv.frw(l.pos);
        l.dir = zn::fig_arr2dir(*i, true);
        if (i->size()>=3){
          l.ang=ang_pfig2a((*i)[1], (*i)[2], l.dir, cnv);
          l.hor=false;
        }
        else{
          l.ang=0;
          l.hor=true;
        }
        labels.push_back(l);
        continue;
      }

      object o;
      o.opts = i->opts;
      o.type = type;
      if (i->comment.size()>0){
        o.text = i->comment[0];
        o.comm.insert(o.comm.begin(),
            i->comment.begin()+1, i->comment.end());
      }
      dLine pts = cnv.line_frw(*i);
      // if closed polyline -> add one more point
      if ((type & zn::line_mask) &&
          (i->is_closed()) &&
          (i->size()>0) &&
          ((*i)[0]!=(*i)[i->size()-1])) pts.push_back(pts[0]);
      o.push_back(pts);
      o.dir=zn::fig_arr2dir(*i);
      // keep objects waiting for a label separately
      if ((zconverter.get_label_pos(type)) && (o.text != "")) o4l.push_back(o);
      else push_back(o);
      continue;
    }
    // find normal labels
    if ((i->opts.exists("MapType")) &&
        (i->opts.get("MapType", std::string())=="label") &&
        (i->opts.exists("RefPt")) ){
      if ((!i->is_text()) || (i->size()<1)) continue;
      lpos_full l;
      l.pos = (*i)[0]; cnv.frw(l.pos);
      l.ref = i->opts.get("RefPt", l.pos); cnv.frw(l.ref);
      l.dir  = i->sub_type;
      l.text = i->text;
      // fix angle (fig->latlon)
      if (i->angle!=0){
        l.ang = ang_afig2a(i->angle, l.dir, cnv, l.pos);
        l.hor = false;
      }
      else {
        l.ang = 0;
        l.hor = true;
      }
      labels.push_back(l);
      continue;
    }
  }
  // find labels for each object
  add_labels(o4l, labels);
  // create new labels
  new_labels(o4l, zconverter);
  insert(begin(), o4l.begin(), o4l.end());
  return 1;
}

// get vmap objects and labels from mp
int
world::get(const mp::mp_world & M){

  // get map data -- TODO
  style = default_style;
  rscale = default_rscale;
  name  = M.Name;
  mp_id = M.ID;

  // get map objects and labels:
  zn::zn_conv zconverter(style);
  std::list<lpos_full> labels; // ownerless labels
  world o4l; // objects waiting for a label
  for (mp::mp_world::const_iterator i=M.begin(); i!=M.end(); i++){
    int type = zconverter.get_type(*i);
    if (type==border_type){ // special type -- border
      brd = join_polygons(*i);
    }
    else if (type==label_type){ // special type -- label objects
      if (i->Label=="") continue;
      for (dMultiLine::const_iterator j=i->begin(); j!=i->end(); j++){
        if (j->size()<2) continue;
        lpos_full l;
        l.text = i->Label;
        l.ref = (*j)[0];
        l.pos = (*j)[1];
        l.dir = i->Opts.get("Direction", 0);
        if (j->size()>=3){
          l.ang=ang_pll2a((*j)[1], (*j)[2], l.dir);
          l.hor = false;
        }
        else{
          l.ang = 0;
          l.hor = true;
        }
        labels.push_back(l);
      }
    }
    else {
      object o;
      o.dMultiLine::operator=(*i);
      o.opts = i->Opts;
      o.type = type;
      o.text = i->Label;
      o.comm = i->Comment;
      o.dir  = i->Opts.get("Direction", 0);

      // keep objects waiting for a label separately
      // TODO -- don't modify zconverter by []
      if ((zconverter.znaki[type].label_pos) && (o.text != "")) o4l.push_back(o);
      else push_back(o);
    }
  }
  // find labels for each object
  add_labels(o4l, labels);
  // create new labels
  new_labels(o4l, zconverter);
  insert(begin(), o4l.begin(), o4l.end());
  return 1;
}

/***************************************/

// put vmap to referenced fig
int
world::put(fig::fig_world & F, bool put_labels, bool fig_text_labels){
  zn::zn_conv zconverter(style);
  g_map ref = fig::get_ref(F);
  if (ref.size()<3){
    std::cerr << "ERR: not a GEO-fig\n"; return 0;
  }
  convs::map2pt cnv(ref, Datum("wgs84"), Proj("lonlat"));

  // save map parameters
  F.opts.put("style",  style);
  F.opts.put("rscale", rscale);
  F.opts.put("name",   name);
  F.opts.put("mp_id",  mp_id);

  // add border
  fig::fig_object brd_o = zconverter.get_fig_template(border_type);
  brd_o.set_points(cnv.line_bck(brd));
  brd_o.close();
  F.comment.push_back("BRD " + name);
  F.push_back(brd_o);

  // add other objects
  for (world::const_iterator o = begin(); o!=end(); o++){

    fig::fig_object fig = zconverter.get_fig_template(o->type);
    fig.comment.push_back(o->text);
    fig.comment.insert(fig.comment.end(), o->comm.begin(), o->comm.end());
    fig.opts = o->opts;

    if (o->type & zn::area_mask){
      fig.set_points(cnv.line_bck(join_polygons(*o)));
      F.push_back(fig);
    } else {
      dMultiLine::const_iterator l;
      for (l=o->begin(); l!=o->end(); l++){
        fig.clear();
        fig.set_points(cnv.line_bck(*l));
        // closed polyline
        if ((o->type & zn::line_mask) &&
            (fig.size()>1) && (fig[0]==fig[fig.size()-1])){
          fig.resize(fig.size()-1);
          fig.close();
        }
        zn::fig_dir2arr(fig, o->dir); // arrows
        // pictures
        std::list<fig::fig_object> tmp=zconverter.make_pic(fig, o->type);
        F.insert(F.end(), tmp.begin(), tmp.end());
      }
    }
    // labels
    if (!put_labels || (o->text == "")) continue;

    std::list<lpos>::const_iterator l;
    for (l=o->labels.begin(); l!=o->labels.end(); l++){
      dPoint ref;  dist_pt_l(l->pos, *o, ref);
      cnv.bck(ref);
      dPoint pos = l->pos;
      cnv.bck(pos);

      fig::fig_object txt;
      if (fig_text_labels){
        txt=zconverter.get_label_template(o->type);
        txt.text=o->text;
        txt.sub_type=l->dir;
        if (l->hor) txt.angle=0;
        else txt.angle=ang_a2afig(l->ang, cnv, pos);
        txt.push_back(pos);
        txt.opts.put("RefPt", ref);
        txt.opts.put("MapType", std::string("label"));
      }
      else {
        txt.clear();
        txt=zconverter.get_fig_template(label_type);
        zn::fig_dir2arr(txt, l->dir, true);
        txt.push_back(ref);
        txt.push_back(pos);
        if (!l->hor)
          txt.push_back(ang_a2pfig(l->ang, l->dir, cnv, pos));
        txt.comment.push_back(o->text);
      }
      F.push_back(txt);
    }
  }
  return 1;
}

// put vmap to mp
int
world::put(mp::mp_world & M, bool put_labels){
  zn::zn_conv zconverter(style);

  // save map parameters
  // TODO - style
  // TODO - rscale
  M.Name = name;
  M.ID = mp_id;

  // add border object
  mp::mp_object brd_o = zconverter.get_mp_template(border_type);
  brd_o.push_back(brd);
  M.push_back(brd_o);

  // add other objects
  for (world::const_iterator o = begin(); o!=end(); o++){
    mp::mp_object mp = zconverter.get_mp_template(o->type);
    mp.dMultiLine::operator=(*o); // copy points
    mp.Label   = o->text;
    mp.Comment = o->comm;
    mp.Opts    = o->opts;

    M.push_back(mp);

    // labels
    if (!put_labels || (o->text == "")) continue;
    std::list<lpos>::const_iterator l;
    for (l=o->labels.begin(); l!=o->labels.end(); l++){
      dPoint ref;  dist_pt_l(l->pos, *o, ref);
      dPoint pos = l->pos;
      mp::mp_object txt=zconverter.get_mp_template(label_type);
      txt.Opts.put("Direction", l->dir); //TODO -- fix
      txt.Label=o->text;
      dLine tmp;
      tmp.push_back(ref);
      tmp.push_back(pos);
      if (!l->hor)
        tmp.push_back(ang_a2pll(l->ang, l->dir, pos));
      txt.push_back(tmp);
      M.push_back(txt);
    }
  }
  return 1;
}

/***************************************/

dPoint
v_m2deg(const dPoint & v, const double lat){
  dPoint ret(v/6380000.0 * 180/M_PI);
  ret.x /= cos(M_PI/180.0 * lat);
  return ret;
}
dPoint
v_deg2m(const dPoint & v, const double lat){
  dPoint ret(v * 6380000 * M_PI/180);
  ret.x *= cos(M_PI/180.0 * lat);
  return ret;
}

double
dist_pt(const dPoint & v, double lat){
  return pdist(v_deg2m(v,lat));
}
double
dist_pt_pt(const dPoint & p1, const dPoint & p2){ // approximate distance in m
  return  pdist(v_deg2m(p2-p1,(p2.y+p1.y)/2.0));
}
double
dist_pt_l(const dPoint & p, const dMultiLine & l, dPoint & n){
  double ret = 1e99;
  for (dMultiLine::const_iterator i=l.begin(); i!=l.end(); i++){
    for (dLine::const_iterator j=i->begin(); j!=i->end(); j++){
      double d = dist_pt_pt(p, *j);
      if (ret > d){ ret=d; n=*j; }
    }
  }
  return ret;
}


/***************************************/

} // namespace
