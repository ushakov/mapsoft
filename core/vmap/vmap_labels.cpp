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

int
add_labels(world & W){
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
new_labels(world & W){
  zn::zn_conv zc(W.style);
  for (world::iterator o=W.begin(); o!=W.end(); o++){
    if ((o->labels.size()>0) || (o->text=="")) continue;

    int lt = zc.get_label_type(o->type);
    int ld = zc.get_label_dir(o->type);
    if (lt==0) continue; // no label for this object

    for (dMultiLine::iterator i=o->begin(); i!=o->end(); i++){

      if (i->size()<1) continue;
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
            l.ang = atan2(v.y, v.x);
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

      if (rotate) o->opts.put<double>("Angle", 180/M_PI*atan2(t.y, t.x));
    }
  }

}

} // namespace
