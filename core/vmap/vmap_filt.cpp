#include <list>
#include <vector>
#include <string>
#include <map>
#include <iostream>

#include "2d/line_utils.h"
#include "2d/line_rectcrop.h"
#include "geo_io/geofig.h"
#include "geo/geo_convs.h"
#include "geo/geo_nom.h"
#include "vmap/zn.h"
#include "vmap.h"

namespace vmap {

using namespace std;

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
change_source(const Options & O, Options &o, const string &name){
}

// crop/cut/select range, get statistics
struct RangeCutter{
  convs::pt2pt *cnv;
  int cnt_o, cnt_l, cnt_p;
  set<string> sources;
  dRect show_range;

  double lon0;
  Datum D;
  Proj P;
  dRect range;
  std::string action;

  RangeCutter(const Options & O){
    // OPTION range_datum lonlat
    // OPTION range_proj  wgs84
    // OPTION range_lon0  0
    // OPTION range
    // OPTION range_nom
    // OPTION range_action

    P = O.get<Proj>("range_proj", Proj("lonlat"));
    D = O.get<Datum>("range_datum", Datum("wgs84"));
    lon0 = O.get<double>("range_lon0", 0);
    range = O.get<dRect>("range");
    action = O.get<string>("range_action");

    cnv=NULL;
    if (action == "") return;

    if (O.exists("range_nom")){
      range=convs::nom_range(O.get<string>("range_nom"));
      cnv = new convs::pt2pt(
          Datum("wgs84"), Proj("lonlat"), Options(),
          Datum("pulkovo"), Proj("lonlat"), Options());
    }
    else {
      Options ProjO;
      ProjO.put<double>("lon0", lon0);
      if (range.x>=1e6){
        double lon0p = convs::lon_pref2lon0(range.x);
        range.x = convs::lon_delprefix(range.x);
        ProjO.put<double>("lon0", lon0p);;
      }
      if (!range.empty())
        cnv = new convs::pt2pt(
          Datum("wgs84"), Proj("lonlat"), Options(), D, P, ProjO);
    }
    if ((cnv==NULL) && (action!="")){
      std::cerr << "warning: can't set geo conversion for range_action="
                << action << "\n";
      std::cerr << "         skipping action\n";
    }
    cnt_o=0; cnt_l=0; cnt_p=0;
  }
  ~RangeCutter(){
    if (cnv)   delete cnv;
  }


  void
  process(object & o){
    if ((cnv==NULL) || (action == "")) return;
    dMultiLine::iterator l = o.begin();
    while (l != o.end()){

      if (l->size()>0){

        bool closed = ( (*l)[0] == (*l)[l->size()-1] );
        dLine lc = cnv->line_frw(*l);
        rect_crop(range, lc, closed);

        if (action == "skip"){
          if (lc.size()!=0) l->clear();
        }
        else if (action == "select"){
          if (lc.size()==0) l->clear();
        }
        else if (action == "crop"){
          *l = cnv->line_bck(lc);
        }
      }
      if (l->size()==0){
        l=o.erase(l);
      }
      else {
        cnt_l++;
        cnt_p+=l->size();
        l++;
      }
    }
    if (o.size()>0) cnt_o++;
    sources.insert(o.opts.get<string>("Source"));
  }

  void
  print_info(){
    cout << "  objects: " << cnt_o << "\n";
    cout << "  lines:   " << cnt_l << "\n";
    cout << "  points:  " << cnt_p << "\n";
    cout << "  sources:";
    for (set<string>::const_iterator i=sources.begin(); i!=sources.end(); i++)
      cout << " " << *i;
    cout << "\n";
  }
};



/***************************************/

void
filter(world & W, const Options & O){

  // OPTION name
  if (O.exists("name")) W.name = O.get<string>("name");

  // OPTION mp_id
  if (O.exists("mp_id")){
    int mp_id = O.get<int>("mp_id", 0);
    if (mp_id<= 0){
      cerr << "error: bad mp_id value: " << O.get<string>("mp_id") << "\n";
      exit(1);
    }
    W.mp_id  = mp_id;
  }

  // OPTION rscale
  if (O.exists("rscale")){
    double rscale = O.get<double>("rscale", 0.0);
    if (rscale <= 0){
      cerr << "error: bad rscale value: " << O.get<string>("rscale") << "\n";
      exit(1);
    }
    W.rscale = rscale;
  }

  // OPTION style
  if (O.exists("style")){
    string style = O.get<string>("style");
    if (style == ""){
      cerr << "error: empty style value\n";
      exit(1);
    }
    W.style  = style;
  }

  // OPTION skip_labels 0
  // OPTION read_labels
  int skip_labels = O.get<int>("skip_labels", 0);
  if (O.get<int>("read_labels", 0)) skip_labels=0;
  if (skip_labels) W.lbuf.clear();

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

  world::iterator o=W.begin();
  while (o!=W.end()){
    // OPTION skip_all
    // OPTION skip_type
    // OPTION select_type
    // OPTION select_source
    // OPTION skip_source
    if (skip_object(O, *o)){ o=W.erase(o); continue; }

    if (skip_labels) o->labels.clear();

    // OPTION set_source_from_name
    // OPTION set_source
    string source = o->opts.get<string>("Source"); // from original object
    if (O.get<int>("set_source_from_name", 0)) source = W.name; // from map name
    set_source(o->opts, O.get("set_source", source));

    // crop, skip, select range
    RC.process(*o);
    if (o->size()==0) o=W.erase(o);
    else o++;
  }
}


} // namespace
