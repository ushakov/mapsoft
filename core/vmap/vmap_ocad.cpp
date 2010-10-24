#include <list>
#include <vector>
#include <string>
#include <map>
#include <iostream>

#include "2d/line_utils.h"
#include "geo/geo_convs.h"
#include "vmap/zn.h"
#include "vmap.h"

#include "ocad/ocad_geo.h"
#include "ocad/ocad_file.h"

namespace vmap {

using namespace std;

/***************************************/

// get vmap objects and labels from ocad
world
read(const ocad::ocad_file & F){
  world ret;

  g_map ref = ocad::get_ref(F);
  if (ref.size()<3){
    std::cerr << "ERR: not a geo-referenced OCAD\n"; return ret;
  }
  convs::map2pt cnv(ref, Datum("wgs84"), Proj("lonlat"));

  // get map data
  ret.rscale = 100000 * convs::map_mpp(ref, Proj("tmerc"));
  ret.style  = default_style;
  ret.name   = "";
  ret.mp_id  = 0;

  // get map objects and labels:
  zn::zn_conv zconverter(ret.style);

  ocad::ocad_objects::const_iterator i;
  for (i=F.objects.begin(); i!=F.objects.end(); i++){

    int otype = i->sym;
    dLine pts = i->line();

    // todo -- labels

    // read map objects

    int type = zconverter.get_type(otype);
    if (!type) continue;

    // special type -- border
    if (type==border_type){ 
      ret.brd = cnv.line_frw(pts);
      continue;
    }

    // normal objects
    object o;
    o.type = type;

    // todo: source, options, direction...

    o.push_back(cnv.line_frw(pts));

    if (o.size()>0) ret.push_back(o);
  }
  return ret;
}

/***************************************/

/*
// put vmap to ocad
int
write(fig::fig_world & F, const world & W, const Options & O){
  // get options
  int append          = O.get<int>("append", 0);          // OPTION append 0
  int fig_text_labels = O.get<int>("fig_text_labels", 1); // OPTION fig_text_labels 1

  zn::zn_conv zconverter(W.style);

  g_map ref = fig::get_ref(F);
  if (ref.size()<3){
    // create ref from brd or from map range
    dLine refs=W.brd;
    if (refs.size()<3){
      refs=rect2line(W.range());
    }
    // add last = first if needed
    if ((refs.size()>0) && (refs[0]!=refs[refs.size()-1])) refs.push_back(refs[0]);
    // reduce border to 5 points, remove last one
    refs=generalize(refs, -1, 5); 
    if (refs.size()>4) refs.resize(4);

    Options PrO;
    PrO.put<double>("lon0", convs::lon2lon0(refs.center().x));
    convs::pt2pt ref_cnv(Datum("wgs84"), Proj("lonlat"), PrO,
                         Datum("wgs84"), Proj("tmerc"), PrO);

    dLine refs_fig(refs);
    ref_cnv.line_frw_p2p(refs_fig);
    refs_fig *= 100.0/W.rscale * fig::cm2fig; // fig units
    refs_fig -= refs_fig.range().BLC();

    ref.clear(); ref.border.clear();
    for (int i=0;i<refs.size();i++){
      refs_fig[i].y*=-1;
      ref.push_back(g_refpoint(refs[i], refs_fig[i]));
      ref.border.push_back(refs[i]);
    }
    ref.map_proj=Proj("tmerc");
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

      fig::fig_object txt;
      if (fig_text_labels){
        txt=zconverter.get_label_template(o->type);
        txt.text=o->text;
        txt.sub_type=l->dir;
        if (l->hor) txt.angle=0;
        else txt.angle=ang_a2afig(l->ang, cnv, pos, W.rscale);
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
        if (!l->hor)
          txt.push_back(ang_a2pfig(l->ang, l->dir, cnv, pos, W.rscale));
        txt.comment.push_back(o->text);
      }
      F.push_back(txt);
    }
  }
  // TODO: write lbuf!
  return 1;
}
*/

} // namespace
