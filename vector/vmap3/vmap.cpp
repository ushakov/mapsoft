#include <list>
#include <vector>
#include <string>
#include <map>
#include <iostream>

#include "lib2d/line_utils.h"
#include "lib2d/line_rectcrop.h"
#include "libgeo_io/geofig.h"
#include "libgeo/geo_convs.h"
#include "libgeo/geo_nom.h"
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
world::add_labels(){
  std::list<lpos_full>::iterator l, l0;
  world::iterator o;
  dPoint p, p0;

  // one nearest label with correct text (label_search_dist1 parameter)
  for (o=begin(); o!=end(); o++){
    if (o->text == "") continue;
    double d0=1e99;
    for (l=lbuf.begin(); l!=lbuf.end(); l++){
      if (l->text != o->text) continue;
      double d = dist_pt_l(l->ref, *o, p);
      if ( d < d0){ d0=d; p0=p; l0=l;}
    }
    if (d0 * 100 / rscale < label_search_dist1){
      o->labels.push_back(*l0);
      lbuf.erase(l0);
    }
  }

  // labels with correct text (label_search_dist2 parameter)
  for (o=begin(); o!=end(); o++){
    if (o->text == "") continue;
    l=lbuf.begin();
    while (l!=lbuf.end()){
      // near ref or near pos
      if ( ((l->text == o->text) &&
            (dist_pt_l(l->ref, *o, p)* 100 / rscale < label_search_dist2)) ||
           ((l->text == o->text) &&
            (dist_pt_l(l->pos, *o, p)* 100 / rscale < label_search_dist2)) ){
        o->labels.push_back(*l);
        l = lbuf.erase(l);
        continue;
      }
      l++;
    }
  }

  // labels with changed text (label_search_dist3 parameter)
  for (o=begin(); o!=end(); o++){
    if (o->text == "") continue;
    l=lbuf.begin();
    while (l!=lbuf.end()){
      if (dist_pt_l(l->ref, *o, p)* 100 / rscale < label_search_dist3){
        o->labels.push_back(*l);
        l = lbuf.erase(l);
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
world::new_labels(zn::zn_conv & zconverter){
  for (world::iterator o=begin(); o!=end(); o++){
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
      l.hor = true;
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
            l.hor = false;
          }
          else if (i->size()==1) l.pos = (*i)[0];
          break;
      }
      o->labels.push_back(l);
    }
  }
}

/***************************************/

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

bool
skip_object(const Options & O, const object &o){
  if (O.get<int>("skip_all", 0)) return true;

  if (O.exists("select_type") && (o.type!=O.get<int>("select_type", 0))) return true;
  if (O.exists("skip_type") && (o.type==O.get<int>("skip_type", 0))) return true;

  string source = o.opts.get<string>("Source");
  if (O.exists("select_source") && (source!=O.get<string>("select_source"))) return true;
  if (O.exists("skip_source") && (source==O.get<string>("skip_source"))) return true;
  return false;
}
void
set_source(Options & o, const string & source){
  if (source != "") o.put<string>("Source", source);
}
void
change_source(const Options & O, Options &o, const string &name){
  string source = o.get<string>("Source"); // from original object
  if (O.get<int>("set_source_from_name", 0)) source = name; // from map name
  set_source(o, O.get("set_source", source));
}


struct RangeCutter{
  dRect skip_range, crop_range, select_range;
  convs::pt2pt *skip_cnv, *crop_cnv, *select_cnv;

  RangeCutter(const Options & O){
    // OPTION skip_range
    // OPTION select_range
    // OPTION crop_range
    // OPTION skip_nom
    // OPTION select_nom
    // OPTION crop_nom
    // OPTION range_datum lonlat
    // OPTION range_proj  wgs84
    // OPTION range_lon0  0

    skip_cnv   = mk_cnv_and_range(O, "skip", skip_range);
    crop_cnv   = mk_cnv_and_range(O, "crop", crop_range);
    select_cnv = mk_cnv_and_range(O, "select", select_range);
  }
  ~RangeCutter(){
    if (skip_cnv) delete skip_cnv;
    if (crop_cnv) delete crop_cnv;
    if (select_cnv) delete select_cnv;
  }

  convs::pt2pt *
  mk_cnv_and_range(const Options & O, const string & prefix, dRect & range){
    double lon0 = O.get<double>("range_lon0", 0);
    Proj  P = O.get<Proj>("range_proj", Proj("lonlat"));
    Datum D = O.get<Datum>("range_datum", Datum("wgs84"));
    convs::pt2pt *cnv = NULL;

    if (O.exists(prefix+"_nom")){
      range=convs::nom_range(O.get<string>(prefix+"_nom"));
      P = Proj("lonlat");
      D = Datum("pulkovo");
    }
    else if (O.exists(prefix+"_range")){
      range = O.get<dRect>(prefix+"_range");
      if (range.x>=1e6){
        double lon0p = convs::lon_pref2lon0(range.x);
        range.x = convs::lon_delprefix(range.x);
        if (lon0p!=0) lon0=lon0p;
      }
    }
    Options ProjO;
    ProjO.put<double>("lon0", lon0);
    if (!range.empty())
        cnv = new convs::pt2pt(
          Datum("wgs84"), Proj("lonlat"), Options(), D, P, ProjO);
    return cnv;
  }

  void
  process(object & o){
    dMultiLine::iterator l = o.begin();
    while (l != o.end()){

      if (l->size() > 0){
        bool closed = ( (*l)[0] == (*l)[l->size()-1] );
        if (skip_cnv){
          dLine lc = skip_cnv->line_frw(*l);
          rect_crop(skip_range, lc, closed);
          if (lc.size()!=0) l->clear();
        }
        if (select_cnv){
          dLine lc = select_cnv->line_frw(*l);
          rect_crop(select_range, lc, closed);
          if (lc.size()==0) l->clear();
        }
        if (crop_cnv){
          dLine lc = crop_cnv->line_frw(*l);
          rect_crop(crop_range, lc, closed);
          *l = crop_cnv->line_bck(lc);
        }
      }

      if (l->size()==0) l=o.erase(l);
      else l++;
    }
  }
};

/***************************************/

// get vmap objects and labels from fig
int
world::get(const fig::fig_world & F, const Options & O){

  // get fig reference
  g_map ref = fig::get_ref(F);
  if (ref.size()<3){
    std::cerr << "ERR: not a GEO-fig\n"; return 0;
  }
  convs::map2pt cnv(ref, Datum("wgs84"), Proj("lonlat"));

  int skip_labels = O.get<int>("skip_labels", 0);     // OPTION skip_labels 0
  if (O.get<int>("read_labels", 0)) skip_labels=0;    // OPTION read_labels

  // get map data
  rscale = 100 * convs::map_mpp(ref, Proj("tmerc")) * fig::cm2fig;
  rscale = F.opts.get<double>("rscale", rscale);
  style  = F.opts.get<string>("style", default_style);
  name   = F.opts.get<string>("name");
  mp_id  = F.opts.get<int>("mp_id");

  // OPTION skip_range
  // OPTION select_range
  // OPTION crop_range
  // OPTION skip_nom
  // OPTION select_nom
  // OPTION crop_nom
  // OPTION range_datum lonlat
  // OPTION range_proj  wgs84
  // OPTION range_lon0  0
  RangeCutter RC(O);

  // get map objects and labels:
  zn::zn_conv zconverter(style);
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
        if (skip_labels) continue;
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
        lbuf.push_back(l);
        continue;
      }

      object o;
      o.type = type;
      set_source(o.opts, i->opts.get<string>("Source"));

      // OPTION skip_all
      // OPTION skip_type
      // OPTION select_type
      // OPTION select_source
      // OPTION skip_source
      if (skip_object(O, o)) continue;

      // OPTION set_source_from_name
      // OPTION set_source
      change_source(O, o.opts, name);

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

      // crop, skip, select range
      RC.process(o);
      if (o.size()>0) push_back(o);
      continue;
    }
    // find normal labels
    if ((i->opts.exists("MapType")) &&
        (i->opts.get("MapType", std::string())=="label") &&
        (i->opts.exists("RefPt")) ){
      if ((!i->is_text()) || (i->size()<1)) continue;
      if (skip_labels) continue;
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
      lbuf.push_back(l);
      continue;
    }
  }
  return 1;
}

// get vmap objects and labels from mp
int
world::get(const mp::mp_world & M, const Options & O){

  // get map data -- TODO
  style =  M.Opts.get("Style", default_style);
  rscale = M.Opts.get("RScale", default_rscale);
  name  = M.Name;
  mp_id = M.ID;

  int skip_labels = O.get<int>("skip_labels", 0);     // OPTION skip_labels 0
  if (O.get<int>("read_labels", 0)) skip_labels=0;    // OPTION read_labels

  // OPTION skip_range
  // OPTION select_range
  // OPTION crop_range
  // OPTION skip_nom
  // OPTION select_nom
  // OPTION crop_nom
  // OPTION range_datum lonlat
  // OPTION range_proj  wgs84
  // OPTION range_lon0  0
  RangeCutter RC(O);

  // get map objects and labels:
  zn::zn_conv zconverter(style);
  for (mp::mp_world::const_iterator i=M.begin(); i!=M.end(); i++){
    int type = zconverter.get_type(*i);
    if (type==border_type){ // special type -- border
      brd = join_polygons(*i);
    }
    else if (type==label_type){ // special type -- label objects
      if (i->Label=="") continue;
      if (skip_labels) continue;
      for (dMultiLine::const_iterator j=i->begin(); j!=i->end(); j++){
        if (j->size()<2) continue;
        lpos_full l;
        l.dir  = i->Opts.get<int>("LabelDirection", 0);
        l.text = i->Opts.get<string>("LabelText");
        l.ref = (*j)[0];
        l.pos = (*j)[1];
        if (i->Opts.exists("LabelAngle")){
          l.ang = i->Opts.get("LabelAngle",0.0);
          l.hor = false;
        }
        else{
          l.ang = 0;
          l.hor = true;
        }
        lbuf.push_back(l);
      }
    }
    else {
      object o;

      o.type = type;
      o.text = i->Label;
      o.comm = i->Comment;
      o.dir  = i->Opts.get("Direction", 0);
      set_source(o.opts, i->Opts.get<string>("Source"));

      // OPTION skip_all
      // OPTION skip_type
      // OPTION select_type
      // OPTION select_source
      // OPTION skip_source
      if (skip_object(O, o)) continue;

      // OPTION set_source_from_name
      // OPTION set_source
      change_source(O, o.opts, name);

      o.dMultiLine::operator=(*i);

      // crop, skip, select range
      RC.process(o);
      if (o.size()>0) push_back(o);
    }
  }
  return 1;
}

/***************************************/

// put vmap to referenced fig
int
world::put(fig::fig_world & F, const Options & O){
  // get options
  int append          = O.get<int>("append", 0);          // OPTION append 0
  int skip_labels     = O.get<int>("skip_labels", 0);     // OPTION skip_labels 0
  int fig_text_labels = O.get<int>("fig_text_labels", 1); // OPTION fig_text_labels 1

  zn::zn_conv zconverter(style);
  // find labels for each object
  add_labels();
  // create new labels
  new_labels(zconverter);


  g_map ref = fig::get_ref(F);
  if (ref.size()<3){
    std::cerr << "ERR: not a GEO-fig\n"; return 0;
  }
  convs::map2pt cnv(ref, Datum("wgs84"), Proj("lonlat"));

  // cleanup fig if not in append mode
  if (!append){
    fig::fig_world::iterator i = F.begin();
    while (i!=F.end()){
      if ((i->type==6) || (i->type==-6) ||
           zn::is_to_skip(*i) ||
           (i->opts.get<string>("MapType") == "label") ||
           zconverter.is_map_depth(*i)) i=F.erase(i);
      else i++;
    }
  }

  // save map parameters
  F.opts.put("style",  style);
  F.opts.put("rscale", rscale);
  F.opts.put("name",   name);
  F.opts.put("mp_id",  mp_id);

  // add border
  if (brd.size()>0){
    fig::fig_object brd_o = zconverter.get_fig_template(border_type);
    brd_o.set_points(cnv.line_bck(brd));
    brd_o.close();
    brd_o.comment.push_back("BRD " + name);
    if (brd.size()!=0) F.push_back(brd_o);
  }

  // add other objects
  for (world::const_iterator o = begin(); o!=end(); o++){
    if (o->size()==0) continue;

    // OPTION skip_all
    // OPTION skip_type
    // OPTION select_type
    // OPTION select_source
    // OPTION skip_source
    if (skip_object(O, *o)) continue;

    fig::fig_object fig = zconverter.get_fig_template(o->type);

    set_source(fig.opts, o->opts.get<string>("Source"));

    // OPTION set_source_from_name
    // OPTION set_source
    change_source(O, fig.opts, name);

    fig.comment.push_back(o->text);
    fig.comment.insert(fig.comment.end(), o->comm.begin(), o->comm.end());

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
    if (skip_labels || (o->text == "")) continue;

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
world::put(mp::mp_world & M, const Options & O){
  zn::zn_conv zconverter(style);
  // find labels for each object
  add_labels();
  // create new labels
  new_labels(zconverter);

  // get options
  int append          = O.get<int>("append",     0);  // OPTION append 0
  int skip_labels     = O.get<int>("skip_labels", 0); // OPTION skip_labels 0

  // save map parameters
  M.Name = name;
  M.ID = mp_id;
  M.Opts.put("RScale", rscale);
  M.Opts.put("Style",   style);

  // cleanup fig if not in append mode
  if (!append) M.clear();

  // add border object
  mp::mp_object brd_o = zconverter.get_mp_template(border_type);
  if (brd.size()!=0){
     brd_o.push_back(brd);
     M.push_back(brd_o);
  }

  // add other objects
  for (world::const_iterator o = begin(); o!=end(); o++){
    if (o->size()==0) continue;

    // OPTION skip_all
    // OPTION skip_type
    // OPTION select_type
    // OPTION select_source
    // OPTION skip_source
    if (skip_object(O, *o)) continue;

    mp::mp_object mp = zconverter.get_mp_template(o->type);

    set_source(mp.Opts, o->opts.get<string>("Source"));

    // OPTION set_source_from_name
    // OPTION set_source
    change_source(O, mp.Opts, name);

    mp.dMultiLine::operator=(*o); // copy points

    mp.Label   = o->text;
    mp.Comment = o->comm;
    M.push_back(mp);

    // labels
    if (skip_labels || (o->text == "")) continue;
    std::list<lpos>::const_iterator l;
    for (l=o->labels.begin(); l!=o->labels.end(); l++){
      dPoint ref;  dist_pt_l(l->pos, *o, ref);
      dPoint pos = l->pos;
      mp::mp_object txt=zconverter.get_mp_template(label_type);
      txt.Opts.put("LabelDirection", l->dir);
      txt.Opts.put("LabelText", o->text);
      if (!l->hor) txt.Opts.put("LabelAngle", l->ang);
      dLine tmp;
      tmp.push_back(ref);
      tmp.push_back(pos);
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
