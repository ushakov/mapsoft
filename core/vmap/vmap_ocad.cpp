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
  convs::map2wgs cnv(ref);

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

const double brd_ocad_acc = 10; // border generalization accuracy in ocad units

// put vmap to ocad
int
write(ocad::ocad_file & F, const world & W, const Options & O){

  // get options
  int append          = O.get<int>("append", 0);          // OPTION append 0
  // append does not work now!

  // read template file.
  string tmpl = string("/usr/share/mapsoft/") + W.style + ".ocd9";
  try {
    F.read(tmpl.c_str());
  }
  catch (Err e) {
    cerr << "Error:  bad ocad file: " << e.get_error() << endl;
    return 0;
  }

  zn::zn_conv zconverter(W.style);

  g_map ref = vmap::mk_tmerc_ref(W, 1000);
  convs::map2wgs cnv(ref);

  dPoint p0(0,0); cnv.frw(p0);
  ocad::set_ref(F, W.rscale, p0);

//  // cleanup objects if not in append mode
//  if (!append) F.objects.clear();

  // save map parameters TODO - get/set pair!
  //ocad::set_vmap(F, W.style, W.rscale, W.name, W.mp_id);

  // add border
  if (W.brd.size()>0){
    F.add_object(
      zconverter.get_ocad_type(border_type),
      // convert and reduce point number
      generalize(cnv.line_bck(W.brd), brd_ocad_acc, -1));
  }

  // add other objects
  for (world::const_iterator o = W.begin(); o!=W.end(); o++){
    if (o->size()==0) continue;
    int ocad_sym = zconverter.get_ocad_type(o->type);
    if (ocad_sym ==0){
      std::cerr << "warning: unknown ocad symbol for type: " << o->type << "\n";
      ocad_sym = -1;
    }
    // todo - source, comments

    double ang=0;
    if (o->opts.exists("Angle")){
      ang = o->opts.get<double>("Angle");
      ang=cnv.angd_bck((*o)[0].center(), ang, 0.01);
    }

    if (o->get_class() == POLYGON){
      iLine line = cnv.line_bck(join_polygons(*o));
      if (o->dir==2) line = line.inv();
      F.add_object(ocad_sym, line, ang, o->text, o->get_class());
    }
    else {
      dMultiLine::const_iterator l;
      for (l=o->begin(); l!=o->end(); l++){
        iLine line = cnv.line_bck(*l);
        if (o->dir==2) line = line.inv();
        F.add_object(ocad_sym, line, ang, o->text, o->get_class());
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

      double ang = l->hor ? 0 : cnv.angd_bck(l->pos, l->ang, 0.01);
      int text_sym = zconverter.get_ocad_label_type(o->type);

      if (text_sym/1000 == 790){
        iLine line;
        // 5 points
        line.push_back(pos);
        line.push_back(pos + dPoint(  0, 200));
        line.push_back(pos + dPoint(600, 200));
        line.push_back(pos + dPoint(600, -200));
        line.push_back(pos + dPoint(  0, -200));
        F.add_object(text_sym, line, ang, o->text);
      }
      else if (text_sym/1000 == 718){
        iLine line;
        line.push_back(pos);
        line.push_back(pos + 1000*dPoint(cos(ang/180*M_PI), sin(ang/180*M_PI)));
        F.add_object(text_sym, line, ang, o->text);
      }
      else {
        iLine line;
        F.add_object(text_sym, line, ang, o->text);
        F.objects[F.objects.size()-1].type=5;
      }

      ocad::ocad_string st;
      st.type=50;
      st.obj=F.objects.size()-1;
      ostringstream ss;
      ss << "\tx" << ref.x
         << "\ty" << ref.y
      ;
      st.data = ss.str();
      F.strings.push_back(st);
    }

    // todo -- labels!
  }
  // TODO: write lbuf!
  return 1;
}


} // namespace
