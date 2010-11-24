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

// remove empty lines and objects
void
remove_empty(world & W){
  world::iterator o = W.begin();
  while (o!=W.end()){
    dMultiLine::iterator l = o->begin();
    while (l != o->end()){
      if (l->size()==0) l=o->erase(l);
      else l++;
    }
    if (o->size()==0) o=W.erase(o);
    else o++;
  }
}


void
remove_tails(world & W, double dist, const dRect & cutter, Conv * cnv){
  if (dist<=0) return;

  world::iterator o;
  for (o=W.begin(); o!=W.end(); o++){
    dMultiLine::iterator l;
    for (l = o->begin(); l != o->end(); l++){
      if (l->size()<1) continue;
      // convert line to cutter coords
      dLine lc = cnv? cnv->line_frw(*l) : *l;

      // проверка, что все точки l лежат близко к границе
      bool close_to_brd = true;
      for (dLine::const_iterator pc=lc.begin(); pc!=lc.end(); pc++){
        if ((fabs(pc->x - cutter.x) > dist) &&
            (fabs(pc->y - cutter.y) > dist) &&
            (fabs(pc->x - cutter.x - cutter.w) > dist) &&
            (fabs(pc->y - cutter.y - cutter.h) > dist)){
          close_to_brd=false;
          break;
        }
      }

      // проверка, что есть другой такой объект, близко к сторонам
      // которого лежат все точки l
      bool close_to_obj = false;
      world::const_iterator o1;
      for (o1=W.begin(); !close_to_obj && (o1!=W.end()); o1++){
        // only objects with the same type and text.
        if ((o->type != o1->type) || (o->text != o1->text)) continue;
        dMultiLine::const_iterator l1;
        for (l1 = o1->begin(); !close_to_obj && (l1!=o1->end()); l1++){
          // skip l==l1
          if ((o==o1) && (l==l1)) continue;

          bool found=true;
          dLine l1c = cnv? cnv->line_frw(*l1) : *l1;
          for (dLine::const_iterator p=lc.begin(); p!=lc.end(); p++){
            dPoint v, pp(*p);

            if (nearest_pt(l1c, v, pp, dist) >= dist){
              found=false;
              break;
            }
          }
          if (found) close_to_obj=true;
        }
      }

      if (close_to_brd && close_to_obj) l->clear();
    }
  }
  remove_empty(W);
}


// crop/cut/select range, get statistics
struct RangeCutter{
  convs::pt2pt *cnv;
  int cnt_o, cnt_l, cnt_p;
  set<string> sources;
  dRect show_range;
  double tail_size;

  double lon0;
  Datum D;
  Proj P;
  dRect range;
  std::string action;
  dLine newbrd;

  RangeCutter(const Options & O){
    P = O.get<Proj>("range_proj", Proj("lonlat"));
    D = O.get<Datum>("range_datum", Datum("wgs84"));
    lon0 = O.get<double>("range_lon0", 0);
    range = O.get<dRect>("range");
    action = O.get<string>("range_action");


    cnv=NULL;

    if (action == "help"){
      std::cout << "range actions:\n"
                << "  skip     -- skip   objects, touching range\n"
                << "  select   -- select objects, touching range (complementary to skip)\n"
                << "  crop     -- crop\n"
                << "  crop_spl -- smartly crop lines to minimize extra segments on the borders\n"
                << "  help     -- show this help\n"
      ;
      return;
    }

    if ((action != "") &&
        (action != "skip") &&
        (action != "select") &&
        (action != "crop") &&
        (action != "crop_spl")){
      std::cerr << "warning: skipping unknown range action: " << action << "\n"
                << "         see --range_action help\n"
      ;
      return;
    }

    if (O.exists("range_nom")){
      range=convs::nom_to_range(O.get<string>("range_nom"));
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

    if (O.get<int>("set_brd_from_range", 0) && cnv && !range.empty()){
      newbrd=cnv->line_bck(rect2line(range), 1e-6);
      newbrd.resize(newbrd.size()-1);
    }

    tail_size = O.get<double>("remove_tails", 0);

    cnt_o=0; cnt_l=0; cnt_p=0;
  }

  ~RangeCutter(){
    if (cnv)   delete cnv;
  }

  void
  process(world & W){
    if ((cnv==NULL) || (action == "")) return;

    world::iterator o;
    for (o=W.begin(); o!=W.end(); o++){
      bool closed = (o->get_class() == POLYGON);
      dMultiLine::iterator l;

      // crop lines
      for (l = o->begin(); l != o->end(); l++){
        if (l->size()<1) continue;
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
        else if (action == "crop_spl"){
          if (o->get_class()==POLYLINE){
            dMultiLine ML = rect_split_cropped(range, lc);
            for (dMultiLine::const_iterator i=ML.begin(); i!=ML.end(); i++){
              l = o->insert(l, cnv->line_bck(*i)) + 1;
            }
            l->clear();
          }
          else{
             *l = cnv->line_bck(lc);
          }
        }
      }
    }

    // remove tails and clear empty lines
    if (tail_size > 0) remove_tails(W, tail_size, range, cnv);
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

  // OPTION set_brd
  if (O.exists("set_brd")){
    W.brd = O.get<dLine>("set_brd");
  }

  // OPTION skip_labels 0
  // OPTION read_labels
  // OPTION split_labels
  int do_skip_labels = O.get<int>("skip_labels", 0);
  if (O.get<int>("read_labels", 0)) do_skip_labels=0;
  int do_split_labels = O.get<int>("split_labels", 0);

  if (do_skip_labels)  remove_labels(W);
  if (do_split_labels) split_labels(W);

  // OPTION range_datum lonlat
  // OPTION range_proj  wgs84
  // OPTION range_lon0  0
  // OPTION range
  // OPTION range_nom
  // OPTION range_action ""
  // OPTION set_brd_from_range 0
  RangeCutter RC(O);
  if (RC.newbrd.size()>0) W.brd=RC.newbrd;

  world::iterator o=W.begin();
  while (o!=W.end()){
    // OPTION skip_all
    // OPTION skip_type
    // OPTION select_type
    // OPTION select_source
    // OPTION skip_source
    if (skip_object(O, *o)){ o=W.erase(o); continue; }

    // OPTION set_source_from_name
    // OPTION set_source
    string source = o->opts.get<string>("Source"); // from original object
    if (O.get<int>("set_source_from_name", 0)) source = W.name; // from map name
    set_source(o->opts, O.get("set_source", source));
    o++;
  }
    // crop, skip, select range
  RC.process(W);
}


} // namespace
