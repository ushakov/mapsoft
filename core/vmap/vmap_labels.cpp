#include <list>
#include <vector>
#include <string>
#include <map>
#include <iostream>
#include <algorithm>

#include "2d/line_utils.h"
#include "2d/line_rectcrop.h"
#include "geo_io/geofig.h"
#include "geo/geo_convs.h"
#include "geo/geo_nom.h"
#include "vmap/zn.h"
#include "vmap.h"

namespace vmap {

using namespace std;

const double label_search_dist1 = 1.0; // cm
const double label_search_dist2 = 1.0;
const double label_search_dist3 = 0.1;
const double label_len          = 0.3;
const double label_new_dist     = 0.1;

const double move_to_dist     = 0.002; // deg; -- inaccurate...

/***************************************/

std::map<std::string,std::string> cnv_lable_map = {
  {"`A",  "\u00C0"}, {"`a",  "\u00E0"},
  {"\'A", "\u00C1"}, {"\'a", "\u00E1"},
  {"^A",  "\u00C3"}, {"^a",  "\u00E2"},
  {"~A",  "\u00C3"}, {"~a",  "\u00E3"},
  {"\"A", "\u00C4"}, {"\"a", "\u00E4"},
  {"AA",  "\u00C5"}, {"aa",  "\u00E5"},
  {"AE",  "\u00C6"}, {"ae",  "\u00E6"},
  {"cC",  "\u00C7"}, {"cc",  "\u00E7"},
  {"`E",  "\u00C8"}, {"\'e", "\u00E8"},
  {"\'E", "\u00C9"}, {"\'e", "\u00E9"},
  {"^E",  "\u00CA"}, {"^e",  "\u00EA"},
  {"\"E", "\u00CB"}, {"\"e", "\u00EB"},
  {"`I",  "\u00CC"}, {"\'i", "\u00EC"},
  {"\'I", "\u00CD"}, {"\'i", "\u00ED"},
  {"^I",  "\u00CE"}, {"^i",  "\u00EE"},
  {"\"I", "\u00CF"}, {"\"i", "\u00EF"},
  {"NN",  "\u00D1"}, {"nn", "\u00F1"},
  {"`O",  "\u00D2"}, {"\'o", "\u00F2"},
  {"\'O", "\u00D3"}, {"\'o", "\u00F3"},
  {"^O",  "\u00D4"}, {"^o",  "\u00F4"},
  {"~O",  "\u00D5"}, {"~o",  "\u00F5"},
  {"\"O", "\u00D6"}, {"\"o", "\u00F6"},
  {"OO",  "\u00D8"}, {"oo",  "\u00F8"},
  {"`U",  "\u00D9"}, {"\'u", "\u00F9"},
  {"\'U", "\u00DA"}, {"\'u", "\u00FA"},
  {"^U",  "\u00DB"}, {"^u",  "\u00FB"},
  {"\"U", "\u00DC"}, {"\"u", "\u00FC"},
  {"\'Y", "\u00DD"}, {"\'y", "\u00FD"},

  {".C",  "\u010A"}, {".c",  "\u010B"},
  {"vC",  "\u010C"}, {"vc",  "\u010D"},
  {"LL",  "\u0141"}, {"ll",  "\u0142"},
  {"\'L", "\u013D"}, {"\'l", "\u013E"},
  {"OE",  "\u0152"}, {"oe",  "\u0153"},
  {"vT",  "\u0164"}, {"\'t",  "\u0165"},
  {"\'Z", "\u0179"}, {"\'z", "\u017A"},
  {".Z",  "\u017B"}, {".z",  "\u017C"},
  {"vZ",  "\u017D"}, {"vz",  "\u017E"}
};

std::string
conv_label(const std::string & str){
  std::string ret;
  for (size_t i = 0; i<str.size(); i++) {
    auto c = str[i];
    if (c!='#') { ret.push_back(c); continue; }
    if (i>=str.size()-1) {
      std::cerr << "conv_label: skipping # at the end: "<< str <<"\n";
      continue;
    }
    i++;
    if (str[i]=='#') {ret+=c; continue;}  // "##"
    if (i>=str.size()-1) {
      std::cerr << "conv_label: skipping # at the end: "<< str <<"\n";
      continue;
    }
    auto ss = str.substr(i,2);
    i++;
    auto ii = cnv_lable_map.find(ss);
    if (ii==cnv_lable_map.end()){
      std::cerr << "conv_label: unknown sequence: #" << ss << " in: "<< str <<"\n";
      continue;
    } // unknown
    ret += ii->second;
  }
  return ret;
}

void
join_labels(world & W){
  std::list<lpos_full>::iterator l, l0;
  world::iterator o;
  dPoint p, p0;
  zn::zn_conv zc(W.style);

  // one nearest label with correct text (label_search_dist1 parameter)
  for (o=W.begin(); o!=W.end(); o++){
    if ((o->text == "") || (zc.get_label_type(o->type)==0)) continue;
    double d0=1e99;
    for (l=W.lbuf.begin(); l!=W.lbuf.end(); l++){
      if (l->text != o->text) continue;
      double d = dist_pt_l(l->ref, *o, p);
      if ( d < d0){ d0=d; p0=p; l0=l;}
    }
    if (d0 * 100 / W.rscale < label_search_dist1){
      l0->fsize -= zc.get_label_template(o->type).font_size;
      o->labels.push_back(*l0);
      W.lbuf.erase(l0);
    }
  }

  // labels with correct text (label_search_dist2 parameter)
  for (o=W.begin(); o!=W.end(); o++){
    if ((o->text == "") || (zc.get_label_type(o->type)==0)) continue;
    l=W.lbuf.begin();
    while (l!=W.lbuf.end()){
      // near ref or near pos
      if ( ((l->text == o->text) &&
            (dist_pt_l(l->ref, *o, p)* 100 / W.rscale < label_search_dist2)) ||
           ((l->text == o->text) &&
            (dist_pt_l(l->pos, *o, p)* 100 / W.rscale < label_search_dist2)) ){
        l->fsize -= zc.get_label_template(o->type).font_size;
        o->labels.push_back(*l);
        l = W.lbuf.erase(l);
        continue;
      }
      l++;
    }
  }

  // labels with changed text (label_search_dist3 parameter)
  for (o=W.begin(); o!=W.end(); o++){
    if ((o->text == "") || (zc.get_label_type(o->type)==0)) continue;
    l=W.lbuf.begin();
    while (l!=W.lbuf.end()){
      if (dist_pt_l(l->ref, *o, p)* 100 / W.rscale < label_search_dist3){
        l->fsize -= zc.get_label_template(o->type).font_size;
        o->labels.push_back(*l);
        l = W.lbuf.erase(l);
        continue;
      }
      l++;
    }
  }

  // clear unused labels
  W.lbuf.clear();
}

/***************************************/

void
split_labels(world & W){
  zn::zn_conv zc(W.style);
  for (world::iterator o = W.begin(); o!=W.end(); o++){
    for (std::list<lpos>::iterator l=o->labels.begin(); l!=o->labels.end(); l++){
      lpos_full ll;
      ll.lpos::operator=(*l);
      ll.fsize += zc.get_label_template(o->type).font_size;
      ll.text = o->text;
      dist_pt_l(l->pos, *o, ll.ref);
      W.lbuf.push_back(ll);
    }
    o->labels.clear();
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

/***************************************/

// create new labels
void
create_labels(world & W){
  zn::zn_conv zc(W.style);
  for (world::iterator o=W.begin(); o!=W.end(); o++){
    if ((o->labels.size()>0) || (o->text=="")) continue;

    int lt = zc.get_label_type(o->type);
    int ld = zc.get_label_dir(o->type);
    if (lt==0) continue; // no label for this object

    for (dMultiLine::iterator i=o->begin(); i!=o->end(); i++){

      if (i->size()<1) continue;
      if (o->type >= zn::line_mask && i->size()<2) continue;
         // do not create labels for >2pt line and area objects
      lpos l;
      l.ang = 0;
      l.dir = ld;
      l.hor = true;
      switch (lt){
        case 1: // TR side of object (max x-y point + (1,-1)*td)
          l.pos = max_xpy(*i) +
            v_m2deg(W.rscale / 100.0 * dPoint(1,1) * label_new_dist/sqrt(2.0), (*i)[0].y);
          break;
        case 2: // TL side of object
          l.pos = max_xmy(*i) +
            v_m2deg(W.rscale / 100.0 * dPoint(-1,1) * label_new_dist/sqrt(2.0), (*i)[0].y);
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
            if (v.x<0) v=-v;
            l.ang = 180.0/M_PI*atan2(v.y, v.x);
            l.pos = cp +
              v_m2deg(W.rscale / 100.0 * dPoint(-v.y, v.x)*label_new_dist, (*i)[0].y);
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

void
remove_labels(world & W){
  W.lbuf.clear();
  for (world::iterator o = W.begin(); o!=W.end(); o++)
    o->labels.clear();
}

/***************************************/

void
move_pics(world & W){
  zn::zn_conv zc(W.style);

  for (map<int, zn::zn>::const_iterator z=zc.znaki.begin(); z!=zc.znaki.end(); z++){
    const vector<int> & mt = z->second.move_to;
    if (mt.size()==0) continue;
    bool rotate  = z->second.rotate;
    int  type    = z->first;

    dMultiLine lines;
    for (world::const_iterator o=W.begin(); o!=W.end(); o++){
      if (std::find(mt.begin(), mt.end(), o->type) != mt.end())
        lines.insert(lines.end(), o->begin(), o->end());
    }
    for (world::iterator o=W.begin(); o!=W.end(); o++){
      if (o->type!=type) continue;
      if (o->size()!=1) continue;
      if ((*o)[0].size()!=1) continue; // only 1-segment, 1-point objects!
      dPoint & p = (*o)[0][0];
      dPoint t(1,0);
      nearest_pt(lines, t, p, move_to_dist);

      if (rotate)
        o->opts.put<int>("Angle", lround(180/M_PI*atan2(t.y, t.x)));
    }
  }

}

} // namespace
