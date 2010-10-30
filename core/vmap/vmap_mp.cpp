#include <list>
#include <string>
#include <map>
#include <iostream>

#include "2d/line_utils.h"
#include "vmap/zn.h"
#include "vmap.h"

namespace vmap {

using namespace std;

/***************************************/

// get vmap objects and labels from mp
world
read(const mp::mp_world & M){
  world ret;

  // get map data -- TODO
  ret.style =  M.Opts.get("Style", default_style);
  ret.rscale = M.Opts.get("RScale", default_rscale);
  ret.name  = M.Name;
  ret.mp_id = M.ID;

  // get map objects and labels:
  zn::zn_conv zconverter(ret.style);
  for (mp::mp_world::const_iterator i=M.begin(); i!=M.end(); i++){
    int type = zconverter.get_type(*i);
    if (type==border_type){ // special type -- border
      ret.brd = join_polygons(*i);
    }
    else if (type==label_type){ // special type -- label objects
      int dir = i->Opts.get<int>("LabelDirection", 0);
      string text = i->Opts.get<string>("LabelText");
      if (text == "") continue;

      for (dMultiLine::const_iterator j=i->begin(); j!=i->end(); j++){
        if (j->size()<2) continue;
        lpos_full l;
        l.dir  = dir;
        l.text = text;
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
        ret.lbuf.push_back(l);
      }
    }
    else {
      object o;
      o.type = type;
      o.text = i->Label;
      o.comm = i->Comment;
      o.dir  = i->Opts.get("Direction", 0);
      set_source(o.opts, i->Opts.get<string>("Source"));
      o.dMultiLine::operator=(*i); // set points
      if (o.size()>0) ret.push_back(o);
    }
  }
  return ret;
}

/***************************************/

// put vmap to mp
int
write(mp::mp_world & M, const world & W, const Options & O){
  zn::zn_conv zconverter(W.style);

  // get options
  int append = O.get<int>("append",     0);  // OPTION append 0

  // save map parameters
  M.Name = W.name;
  M.ID = W.mp_id;
  M.Opts.put("RScale", W.rscale);
  M.Opts.put("Style",   W.style);

  // cleanup fig if not in append mode
  if (!append) M.clear();

  // add border object
  mp::mp_object brd_o = zconverter.get_mp_template(border_type);
  if (W.brd.size()!=0){
     brd_o.push_back(W.brd);
     M.push_back(brd_o);
  }

  // add other objects
  for (world::const_iterator o = W.begin(); o!=W.end(); o++){
    if (o->size()==0) continue;

    mp::mp_object mp = zconverter.get_mp_template(o->type);

    set_source(mp.Opts, o->opts.get<string>("Source"));

    mp.dMultiLine::operator=(*o); // copy points

    mp.Label   = o->text;
    mp.Comment = o->comm;
    M.push_back(mp);

    // labels
    if (o->text == "") continue;
    std::list<lpos>::const_iterator l;
    for (l=o->labels.begin(); l!=o->labels.end(); l++){
      dPoint ref;  dist_pt_l(l->pos, *o, ref);
      dPoint pos = l->pos;
      mp::mp_object txt=zconverter.get_mp_template(label_type);
      txt.Opts.put("LabelRef", ref);
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
  // TODO: write lbuf!
  return 1;
}

} // namespace
