#include <list>
#include <vector>
#include <string>
#include <map>
#include <iostream>

#include "2d/line_utils.h"
#include "geo_io/geofig.h"
#include "geo/geo_convs.h"
#include "geo/geo_nom.h"
#include "vmap/zn.h"
#include "vmap.h"

namespace vmap {

using namespace std;

/***************************************/

// get vmap objects and labels from fig
world
read(const fig::fig_world & F){
  world ret;

  // get fig reference
  g_map ref = fig::get_ref(F);
  if (ref.size()<3){
    std::cerr << "ERR: not a GEO-fig\n"; return ret;
  }
  convs::map2pt cnv(ref, Datum("wgs84"), Proj("lonlat"));

  // get map data
  ret.rscale = 100 * convs::map_mpp(ref, Proj("tmerc")) * fig::cm2fig;
  ret.rscale = F.opts.get<double>("rscale", ret.rscale);
  ret.style  = F.opts.get<string>("style", default_style);
  ret.name   = F.opts.get<string>("name");
  ret.mp_id  = F.opts.get<int>("mp_id", 0);

  // get map objects and labels:
  zn::zn_conv zconverter(ret.style);
  std::vector<std::string> cmp_comm, comm;
  for (fig::fig_world::const_iterator i=F.begin(); i!=F.end(); i++){
    if (i->type==6)  cmp_comm = i->comment;
    if (i->type==-6) cmp_comm.clear();
    if (zn::is_to_skip(*i)) continue;

    // find normal labels
    if ((i->opts.exists("MapType")) &&
        (i->opts.get("MapType", std::string())=="label") &&
        (i->opts.exists("RefPt")) &&
        (i->is_text()) &&
        (i->size()>0) ){
      lpos_full l;
      l.pos = (*i)[0]; cnv.frw(l.pos);
      l.ref = i->opts.get("RefPt", l.pos); cnv.frw(l.ref);
      l.dir  = i->sub_type;
      l.text = i->text;
      if (i->angle!=0){
        // angle is inverted becouse of y inversion
        l.ang = -cnv.angd_frw((*i)[0], 180/M_PI*i->angle, 1000);
        l.hor = false;
      }
      else {
        l.ang = 0;
        l.hor = true;
      }
      ret.lbuf.push_back(l);
      continue;
    }


    // read map objects
    if (!zconverter.is_map_depth(*i)) continue;

    int type = zconverter.get_type(*i);
    if (!type) continue;

    // copy comment from compound to the first object:
    if (cmp_comm.size()>0){
      comm=cmp_comm;
      cmp_comm.clear();
    }
    else{
      comm=i->comment;
    }

    // special type -- border
    if (type==border_type){ 
      ret.brd = cnv.line_frw(*i);
      continue;
    }

    // special type -- label objects
    if (type==label_type){
      if (i->size()<2) continue;
      if (comm.size()<1) continue;
      lpos_full l;
      l.text = comm[0];
      l.ref = (*i)[0]; cnv.frw(l.ref);
      l.pos = (*i)[1]; cnv.frw(l.pos);
      l.dir = zn::fig_arr2dir(*i, true);
/*      if (i->size()>=3){
        dPoint dp=(*i)[2]-(*i)[1];
        l.ang=cnv.angd_frw((*i)[0], 180/M_PI*atan2(dp.y, dp.x), 1000);
        l.hor=false;
      }*/
      if (i->opts.exists("TxtAngle")){
        double angle = i->opts.get<double>("TxtAngle", 0);
        l.ang=cnv.angd_frw((*i)[0], angle, 1000);
        l.hor=false;
      }
      else{
        l.ang=0;
        l.hor=true;
      }
      ret.lbuf.push_back(l);
      continue;
    }

    // normal objects
    object o;
    o.type = type;
    set_source(o.opts, i->opts.get<string>("Source"));

    if (comm.size()>0){
      o.text = comm[0];
      o.comm.insert(o.comm.begin(),
          comm.begin()+1, comm.end());
    }
    dLine pts = cnv.line_frw(*i);
    // if closed polyline -> add one more point
    if ((o.get_class() == POLYLINE) &&
        (i->is_closed()) &&
        (i->size()>0) &&
        ((*i)[0]!=(*i)[i->size()-1])) pts.push_back(pts[0]);
    o.push_back(pts);
    o.dir=zn::fig_arr2dir(*i);

    if (o.size()>0) ret.push_back(o);
  }
  return ret;
}

/***************************************/

const double brd_fig_acc = 10; // border generalization accuracy in fig units

// put vmap to referenced fig
int
write(fig::fig_world & F, const world & W, const Options & O){
  // get options
  int append          = O.get<int>("append", 0);          // OPTION append 0
  int fig_text_labels = O.get<int>("fig_text_labels", 1); // OPTION fig_text_labels 1

  zn::zn_conv zconverter(W.style);

  g_map ref = fig::get_ref(F);
  if (ref.size()<3){
    ref = vmap::get_tmerc_rec(W, fig::cm2fig, true);
    fig::rem_ref(F);
    fig::set_ref(F, ref, Options());
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
  F.opts.put("style",  W.style);
  F.opts.put("rscale", W.rscale);
  F.opts.put("name",   W.name);
  F.opts.put("mp_id",  W.mp_id);

  // add border
  if (W.brd.size()>0){
    fig::fig_object brd_o = zconverter.get_fig_template(border_type);
    // convert and reduce point number
    brd_o.set_points(generalize(cnv.line_bck(W.brd), brd_fig_acc, -1));
    brd_o.close();
    brd_o.comment.push_back("BRD " + W.name);
    if (W.brd.size()!=0) F.push_back(brd_o);
  }

  // add other objects
  for (world::const_iterator o = W.begin(); o!=W.end(); o++){
    if (o->size()==0) continue;

    fig::fig_object fig = zconverter.get_fig_template(o->type);
    set_source(fig.opts, o->opts.get<string>("Source"));

    fig.comment.push_back(o->text);
    fig.comment.insert(fig.comment.end(), o->comm.begin(), o->comm.end());

    if (o->opts.exists("Angle")){
      double a = o->opts.get<double>("Angle");
      a=cnv.angd_bck((*o)[0].center(), a, 0.01);
      fig.opts.put<double>("Angle", a);
    }

    if (o->get_class() == POLYGON){
      fig.set_points(cnv.line_bck(join_polygons(*o)));
      F.push_back(fig);
    } else {
      dMultiLine::const_iterator l;
      for (l=o->begin(); l!=o->end(); l++){
        fig.clear();
        fig.set_points(cnv.line_bck(*l));
        // closed polyline
        if ((o->get_class() == POLYLINE) &&
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
    if (o->text == "") continue;
    std::list<lpos>::const_iterator l;
    for (l=o->labels.begin(); l!=o->labels.end(); l++){
      dPoint ref;  dist_pt_l(l->pos, *o, ref);
      cnv.bck(ref);
      dPoint pos = l->pos;
      cnv.bck(pos);

      double angle = l->hor ? 0 : -cnv.angd_bck(l->pos, l->ang, 0.01);

      fig::fig_object txt;
      if (fig_text_labels){
        txt=zconverter.get_label_template(o->type);
        txt.text=o->text;
        txt.sub_type=l->dir;
        txt.angle=M_PI/180*angle;
        txt.push_back(pos);
        txt.opts.put<iPoint>("RefPt", ref);
        txt.opts.put<string>("MapType", "label");
      }
      else {
        txt.clear();
        txt=zconverter.get_fig_template(label_type);
        zn::fig_dir2arr(txt, l->dir, true);
        txt.push_back(ref);
        txt.push_back(pos);
        if (!l->hor) txt.opts.put<double>("TxtAngle", angle);
        txt.comment.push_back(o->text);
      }
      F.push_back(txt);
    }
  }
  // TODO: write lbuf!
  return 1;
}

} // namespace
