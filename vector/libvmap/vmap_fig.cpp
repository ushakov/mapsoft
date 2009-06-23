#include <sstream>

#include "vmap.h"

#include "../libzn/zn_key.h" // for backward compat. read old-style keys
#include <boost/lexical_cast.hpp>


using namespace std;
namespace vmap {

// move comment from compound to the first object in it
void copy_fig_comment(const fig::fig_world::iterator & i, const fig::fig_world::iterator & end){
  if ((i->type == 6) && (i->comment.size()>0)){ // compound with comment

    fig::fig_world::iterator j=i; j++;

    // skip objects with [skip] in the first line of comment
    while ((j!=end) && (j->comment.size()>1) && (j->comment[1]=="[skip]")) j++;

    if (j==end) return;

    if (j->comment.size()< i->comment.size()) j->comment.resize(i->comment.size());
    for (int n=0; n<i->comment.size(); n++) j->comment[n] = i->comment[n];
  }
}

fig::fig_world  vmap2fig(const world & w){
  fig::fig_world FIG;

  if (w.opts.exists("ID"))   FIG.opts.put("ID", w.opts.get("ID", 0));
  if (w.opts.exists("Name")) FIG.opts.put("Name", w.opts.get("Name", string()));
  FIG.comment=w.comm;

  string style=w.opts.get("Style", string("mmb"));
  FIG.opts.put("style", style);

  legend leg(style);

  // TODO: rmap section -> labels

  // TODO: geo-ref, cnv object.
/*
  convs:pt2pt cnv ...

  for (map<id_t, object>::const_iterator o=w.objects.begin(); o!=w.objects.end(); o++){

    fig::fig_object figobj=leg.def_fig;
    string type=o->second.opts.get("type", string());

    legend::const_iterator t=leg.find(type);
    if (t!=leg.end()) mpobj=t->second.mp;

    // TODO: options in fig
    figobj.opts.put("ID", o->first);
    figobj.comment=o->second.comm;

    if (t->mp.Class == "POLYGON"){
      figobj.set_points(cnv.line_bck(join_polygons(*o)));
      FIG.push_back(mpobj);
    }
    else{
      for (dMultiLine::const_iterator s=o->second.data.begin(); 
                                             s!=o->second.data.end(); s++){
        figobj.clear();
        figobj.set_points(cnv.line_bck(*i));
        FIG.push_back(mpobj);
      }
    }

  }
*/
  return FIG;
}

world fig2vmap(const fig::fig_world & FIG){

  world w;
  fig::fig_world FIG1=FIG; // we need to copy all the data to fix comments
  w.comm=FIG1.comment;

  if (FIG1.opts.exists("id"))    w.opts.put("ID", FIG1.opts.get("id",string()));
  if (FIG1.opts.exists("name"))  w.opts.put("Name", FIG1.opts.get("name",string()));

  string style=FIG1.opts.get("style", string("mmb"));
  w.opts.put("Style", style);

  legend leg(style);

  g_map ref = fig::get_ref(FIG1);
  if (ref.size()<3){
    cerr << "ERR: not a GEO-fig\n"; return world();
  }
  convs::map2pt cnv(ref, Datum("wgs84"), Proj("lonlat"));

  for (fig::fig_world::iterator o=FIG1.begin(); o!=FIG1.end(); o++){
    // move comment from compound to the first object in it
    copy_fig_comment(o, FIG1.end());
    if (!leg.is_map_depth(*o)) continue;

    // BC!! old-style keys
    zn::zn_key k=zn::get_key(*o);
    zn::clear_key(*o);
    id_t id=boost::lexical_cast<id_t>(k.id);  // BC!!
    if (k.map!="") id+="@" + k.map;

    // new-style ID
    id=o->opts.get("ID", id);
    if ((id=="")||(id=="0")) id=make_id();

    while (w.objects.find(id)!=w.objects.end()){
      std::cerr << "map::read_mp: duplicated ID.\n";
      id=make_id();
    }

    vmap::object mapobj;

    //label is in a first comment line
    if (o->comment.size()>0){
      if (o->comment[0]!="") mapobj.opts.put("label", o->comment[0]);
      o->comment.erase(o->comment.begin());
    }

    mapobj.comm=o->comment;

    string type=leg.get_type(*o);
    if (type!="") mapobj.opts.put("type", type);
    else std::cerr << "map::read_fig: Unknown type for FIG\n";

    mapobj.data.push_back(cnv.line_frw(*o));
    w.objects[id]=mapobj;

  }

  rmap rm;
  if (FIG1.opts.exists("proj"))  rm.opts.put("Proj", FIG1.opts["proj"]);

  // read labels
  for (fig::fig_world::iterator o=FIG1.begin(); o!=FIG1.end(); o++){

    // BC!! old-style keys
    zn::zn_label_key k=zn::get_label_key(*o);
    id_t id=boost::lexical_cast<id_t>(k.id);  // BC!!
    if (k.map!="") id+="@" + k.map;
    zn::clear_key(*o);

    // new-style ID
    id=o->opts.get("LabelID", id);
    if ((id=="")||(id=="0")) continue;

    if (o->size()<1) continue;
    vmap::label_pos pos;

    pos.x=(*o)[0].x;
    pos.y=(*o)[0].y;
    cnv.frw_safe(pos);
    pos.angle=o->angle*180/M_PI;

    rm.positions.insert(std::pair<id_t, label_pos>(id, pos));
  }

  w.rmaps.insert(std::pair<id_t, rmap>("default", rm));
  return w;
}

} // namespace
