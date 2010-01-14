#include <sstream>

#include "vmap.h"

#include "../libzn/zn_key.h" // for backward compat. read old-style keys
#include "../../core/lib2d/line_utils.h"
#include "../../core/lib2d/line_rectcrop.h"
#include "legend.h"


using namespace std;
namespace vmap {

bool test_skip(const fig::fig_object & o){
  return ((o.comment.size()>1) && (o.comment[1]=="[skip]")) || // BC!!!
         (o.opts.get("Skip", string()) != "");
}

// move comment from compound to the first object in it
void copy_fig_comment(const fig::fig_world::iterator & i, const fig::fig_world::iterator & end){
  if ((i->type == 6) && (i->comment.size()>0)){ // compound with comment

    fig::fig_world::iterator j=i; j++;

    // skip objects with Skip option
    while ((j!=end) && test_skip(*j)) j++;
    if (j==end) return;

    if (j->comment.size()< i->comment.size()) j->comment.resize(i->comment.size());
    for (int n=0; n<i->comment.size(); n++) j->comment[n] = i->comment[n];
  }
}

/**************************************************************************/

Options read_fig(const string & fig_file, vmap & M){
  M=vmap();


  fig::fig_world F;
  if (!fig::read(fig_file.c_str(), F)){
    cerr << "Error: bad fig file " << fig_file << "\n";
    return Options();
  }

  g_map ref = fig::get_ref(F);
  if (ref.size()<3){
    cerr << "Error: not a GEO-fig\n";
    return Options();
  }

  string style=F.opts.get("style", string("mmb"));
  legend leg(style);

  convs::map2pt cnv(ref, Datum("wgs84"), Proj("lonlat"));

  set<string> id_dup; // for duplicated ids

  for (fig::fig_world::iterator i=F.begin(); i!=F.end(); i++){

    copy_fig_comment(i, F.end());
    if (!leg.is_map_depth(*i)) continue;

    mobj_t o;

    //label is in a first comment line
    string label;
    if (i->comment.size()>0)
      label=i->comment[0];
    if (label!="") o.opts.put("Label", label);

    string type=leg.get_type(*i);
    if (type!="") o.opts.put("Type", type);
    else std::cerr << "Warning: unknown object type\n";

    // add point if line is closed
    if ((i->is_closed()) &&
        (i->size()>0) &&
        ((*i)[0]!=(*i)[i->size()-1]))  i->push_back((*i)[0]);

    // arrows
    int dir=0;
    if ((i->forward_arrow==1)&&(i->backward_arrow==0)) dir=1;
    if ((i->forward_arrow==0)&&(i->backward_arrow==1)) dir=2;
    if (dir!=0) o.opts.put("Dir", dir);

    dLine pts=*i;
    cnv.line_frw_p2p(pts);
    o.push_back(pts);

    // BC!! old-style keys
    zn::zn_key k=zn::get_key(*i);
    zn::clear_key(*i);
    string id = boost::lexical_cast<string>(k.id);  // BC!!
    if (k.map!="") id+="@" + k.map;

    // new-style ID
    id=i->opts.get("ID", id);
    if ((id=="")||(id=="0")) id=make_id();

    if (i->comment.size()>0)
      i->comment.erase(i->comment.begin()); // clear label;
    o.comm=i->comment;

    // duplicated id's
    if (M.mobjs.count(id)>0){
      // if object have the same parameters append it to existing one
      if ((M.mobjs[id].opts.get("Label", string()) == label) &&
          (M.mobjs[id].opts.get("Type",  string())  == type) &&
          (M.mobjs[id].opts.get("Dir",  0)   == dir)){
        M.mobjs[id].push_back(o[0]);
      }
      // else create new object with new id
      else {
        std::cerr << "Warning: duplicated ID \"" << id << "\"\n";
        id_dup.insert(id);
        id=make_id();
        M.mobjs[id]=o;
      }
    }
    else {
      M.mobjs[id]=o;
    }
  }

  string lpos_id=F.opts.get("LPOS", string("DEFAULT"));
  if (M.lpos.count(lpos_id)<1){
    M.lpos[lpos_id]=lpos_t();
  }

  // read labels
  for (fig::fig_world::iterator i=F.begin(); i!=F.end(); i++){
    if (i->type!=4) continue;

    // BC!! old-style keys
    zn::zn_label_key k=zn::get_label_key(*i);
    string id=boost::lexical_cast<string>(k.id);  // BC!!
    if (k.map!="") id+="@" + k.map;
    zn::clear_key(*i);

    // new-style ID
    id=i->opts.get("LabelID", id);
    if ((id=="")||(id=="0")) continue;

    // trow out label positions for duplicated ids
    if (id_dup.count(id)>0) continue;

    if (i->size()<1) continue;

    lp_t lp;
    lp.x=(*i)[0].x;
    lp.y=(*i)[0].y;
    cnv.frw(lp);
    lp.angle=i->angle*180/M_PI;
    lp.align=i->sub_type;
    M.lpos[lpos_id].insert(pair<string,lp_t>(id,lp));
  }
  return F.opts;
}


int put_fig(const string & fig_file, vmap & M, bool replace){

  vmap M1;
  Options o=read_fig(fig_file, M1);

  if ((o.get("edit", string()) == "yes") && (replace)){
    // create cutter range
    g_map ref=fig::ref_from_options(o);
    convs::map2pt cnv(ref, Datum("wgs84"), Proj("lonlat"), Options());
    dRect llcutter=cnv.bb_frw(ref.border.range());

    // cut old objects
    cerr << "Removing old objects in " << llcutter << "\n";
    M.cut_inner(llcutter, true);

  } else if (replace) {
    cerr << "Error: file is not sutable for replace mode!\n";
    exit(1);
  }

  M.add(M1);
  return 0;
}


/**************************************************************************/
int get_fig(const string & fig_file, const vmap & M,
            const bool toedit, const string & rmap_id){


  // find RMAP
  if (M.rmaps.count(rmap_id)<1){
    cerr << "Error: rmap \"" << rmap_id << "\" does not exists\n";
    exit(-1);
  }
  rmap_t rmap=M.rmaps.find(rmap_id)->second;

  // find LPOS
  string lpos_id=rmap.opts.get("LPOS", string("DEFAULT"));
  if (M.lpos.count(lpos_id)<1){
    cerr << "Error: lpos \"" << lpos_id << "\" does not exists\n";
    exit(-1);
  }
  lpos_t lpos=M.lpos.find(lpos_id)->second;

  // creating legend object
  string style=rmap.opts.get("style", string("mmb"));
  legend leg(style);

  // prepare fig file and create map reference for it
  fig::fig_world F;

  if (!fig::read(fig_file.c_str(), F))
    cerr << "creating new file: " << fig_file << "\n";

  F.opts.put("rmap", rmap_id);
  F.opts=rmap.opts;
  F.opts["edit"] = toedit? "yes" : "";

  // remove objects, labels, pictures
  fig::fig_world::iterator o=F.begin();
  while (o!=F.end()){
    if (leg.is_map_depth(*o) ||  // map object
        (zn::get_label_key(*o).map!="") || // BC!!! old-style label
        (o->opts.get("LabelID", string()) != "") || // new-style label
        test_skip(*o)) // objects with [skip] label
           o=F.erase(o);
    else o++;
  }
  fig::rem_ref(F);
  F.remove_empty_comp();

  g_map ref=fig::ref_from_options(rmap.opts);
  fig::set_ref(F, ref, rmap.opts);
  convs::map2pt cnv(ref, Datum("wgs84"), Proj("lonlat"), Options());

  // add border object
  if (toedit){
    fig::fig_object brd = leg.default_fig;
    legend::iterator brd_leg=leg.find("map border");
    if ((brd_leg!=leg.end()) && (brd_leg->second.fig.size()>0))
      brd=brd_leg->second.fig[0];
    brd.set_points(ref.border);
    brd.close();
    brd.opts["Skip"]="yes";
    F.push_back(brd);
  }
  dRect cutter=ref.border.range();
  dRect llcutter=cnv.bb_frw(cutter);

  // processing map objects
  for (map<string, mobj_t>::const_iterator o=M.mobjs.begin(); o!=M.mobjs.end(); o++){

    // skip line outside llcutter (not accurate, but it is enough)
    bool skip=true;
    for (dMultiLine::const_iterator i=o->second.begin();
                                    i!=o->second.end(); i++){
      if (rect_crop_test(llcutter, *i, true)) skip=false;
    }
    if (skip) continue;

    string id    = o->first;
    string type  = o->second.opts.get("Type", string());
    string label = o->second.opts.get("Label", string());
    int dir      = o->second.opts.get("Dir",0);

    fig::fig_object fig  = leg.default_fig;
    fig::fig_object figl;

    if ((leg.count(type)>0) && (leg[type].fig.size()>0))
      fig=leg[type].fig[0];
    if (leg.count(type)>0)
      figl=leg[type].txt;

    //comments
    fig.comment.push_back(label);
    fig.comment.insert(fig.comment.end(), o->second.comm.begin(),
                                          o->second.comm.end());
    fig.opts["ID"]=id;

    // add arrows
    if      (dir==1){fig.forward_arrow=1;  fig.backward_arrow=0;}
    else if (dir==2){fig.backward_arrow=1; fig.forward_arrow=0;}
    else    {fig.backward_arrow=0; fig.forward_arrow=0;}

    // add points (objects can be duplicated)
    dMultiLine::const_iterator i=o->second.begin();
    while ( i!=o->second.end()){

      dLine pts;
      // convert points
      if (fig.area_fill != -1){ // filled polygon -- join segments
        pts=join_polygons(o->second);
        i=o->second.end(); // stop iteration
      }
      else {
        fig.clear();
        pts=*i;
        i++;
      }
      cnv.line_bck_p2p(pts);

      if (!toedit) rect_crop(cutter, pts, fig.area_fill!=-1);

      if (pts.size()==0) continue;
      fig.set_points(pts);

      // close line
      if ((fig.size()>1) && (fig[0]==fig[fig.size()-1])){
        fig.resize(fig.size()-1);
        fig.close();
      }

      // find labels
      std::list<fig::fig_object> fl1;
      for (lpos_t::const_iterator l  = lpos.lower_bound(id);
                                  l != lpos.upper_bound(id); l++){
        dPoint p=l->second;
        cnv.bck(p);
        if (toedit || point_in_rect(p, cutter)){
          figl.clear();
          figl.push_back(p);
          figl.sub_type=l->second.align;
          figl.angle=l->second.angle*M_PI/180;
          figl.opts["LabelID"]=id;
          figl.text=label;
          fl1.push_back(figl);
        }
      }
      if (fl1.size()==0){
        fl1=leg.make_labels(fig,type);
      }
      // make pics
      std::list<fig::fig_object> fl2=leg.make_pic(fig,type);
      F.insert(F.end(), fl1.begin(),fl1.end());
      F.insert(F.end(), fl2.begin(),fl2.end());
    }
  }

  if (!fig::write(fig_file, F))
    exit(-1);
  return F.size();
}
/**************************************************************************/
} // namespace
