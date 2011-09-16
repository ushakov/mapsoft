#include <vector>
#include <cmath>

#include "layer_trk.h"
#include "utils/cairo_wrapper.h"
#include "2d/line_utils.h"

using namespace std;

LayerTRK::LayerTRK(geo_data * _world):
      world(_world), mymap(get_myref()),
      cnv(get_myref(), Datum("wgs84"), Proj("lonlat")),
      myrange(rect_pump(cnv.bb_bck(world->range_geodata()), 1.0)){

#ifdef DEBUG_LAYER_TRK
  cerr  << "LayerTRK: set_ref range: " << myrange << "\n";
#endif
}

void
LayerTRK::refresh(){
  myrange = rect_pump(cnv.bb_bck(world->range_geodata()), 1.0);
}

g_map
LayerTRK::get_ref() const {
  return mymap;
}

g_map
LayerTRK::get_myref() const {
  g_map ret;
  ret.map_proj = Proj("lonlat");

  ret.push_back(g_refpoint(0,  45, 0, 45*3600));
  ret.push_back(g_refpoint(180, 0, 180*3600,0));
  ret.push_back(g_refpoint(0,   0, 0, 90*3600));
  ret.border=rect2line(world->range_geodata());
  return ret;
}

void
LayerTRK::set_ref(const g_map & map){
  mymap=map;
  cnv = convs::map2pt(mymap, Datum("wgs84"), Proj("lonlat"));
  myrange=cnv.bb_bck(world->range_geodata(), 1.0);
#ifdef DEBUG_LAYER_TRK
  cerr  << "LayerTRK: set_ref range: " << myrange << "\n";
#endif
}

iImage
LayerTRK::get_image (iRect src){
  if (rect_intersect(myrange, src).empty()) {
    return iImage(0,0);
  }
  iImage ret(src.w, src.h, 0);
  draw(src.TLC(), ret);
  return ret;
}

void
LayerTRK::draw(const iPoint origin, iImage & image){
  iRect src_rect = image.range() + origin;
#ifdef DEBUG_LAYER_TRK
  cerr  << "LayerTRK: draw " << src_rect <<  " my: " << myrange << "\n";
#endif
  // FIXME - use correct range instead of +110
  if (rect_intersect(myrange, rect_pump(src_rect,110)).empty()) return;

  CairoWrapper cr(image);
  cr->set_line_cap(Cairo::LINE_CAP_ROUND);

  for (vector<g_track>::const_iterator it = world->trks.begin();
                                     it!= world->trks.end(); it++){

    int w = it->width;
    int arr_w = w * 2.0;
    int dot_w = w * 0.5;
    int arr_dist = w * 10; // distance between arrows
    Color color = it->color;

    cr->set_color(color.value);
    cr->set_line_width(w);

    dPoint po;
    vector<g_trackpoint>::const_iterator pt;
    double len_cnt;
    for (pt = it->begin(); pt!= it->end(); pt++){
      dPoint p(*pt); cnv.bck(p);  p-=origin;
      if (pt == it->begin()) po=p;

      iRect r = rect_pump(iRect(p,po), w);

      if (!rect_intersect(r, image.range()).empty()){
        if ((pt == it->begin()) || pt->start){
          cr->circle(p, dot_w);
          len_cnt=0;
        }
        else {
          cr->move_to(po);
          cr->line_to(p);

          if (len_cnt < arr_dist){ // draw dot
            cr->circle(p, dot_w);
            len_cnt+=pdist(po-p);
          }
          else { // draw arrow
            dPoint dp = pnorm(po-p) * arr_w;
            dPoint p1 = p + dp + dPoint(dp.y, -dp.x) * 0.5;
            dPoint p2 = p + dp - dPoint(dp.y, -dp.x) * 0.5;
            cr->move_to(p);
            cr->line_to(p1);
            cr->line_to(p2);
            cr->line_to(p);
            len_cnt=0;
          }

        }
      }
      po=p;
    }
    cr->stroke();
  }
}

pair<int, int>
LayerTRK::find_trackpoint (iPoint pt, int radius){
iRect target_rect (pt,pt);
  target_rect = rect_pump(target_rect, radius);
  for (int track = 0; track < world->trks.size(); ++track) {
    for (int tpt = 0; tpt < world->trks[track].size(); ++tpt) {
      dPoint p(world->trks[track][tpt].x,world->trks[track][tpt].y);
      cnv.bck(p);

      if (point_in_rect(iPoint(p), target_rect)){
        return make_pair(track, tpt);
      }
    }
  }
  return make_pair(-1,-1);
}

pair<int, int>
LayerTRK::find_track (iPoint pt, int radius){
  iRect target_rect (pt,pt);
  target_rect = rect_pump(target_rect, radius);

  for (int track = 0; track < world->trks.size(); ++track) {
    int ts = world->trks[track].size();
    if (ts>0){
      dPoint p(world->trks[track][0].x,world->trks[track][0].y);
      cnv.bck(p);
      if (point_in_rect(iPoint(p), target_rect)){
        return make_pair(track, -2);
      }
      p = dPoint(world->trks[track][ts-1].x,world->trks[track][ts-1].y);
      cnv.bck(p);
      if (point_in_rect(iPoint(p), target_rect)){
        return make_pair(track, -3);
      }
    }
    for (int tpt = 0; tpt < ts-1; ++tpt) {
      dPoint p1(world->trks[track][tpt].x,world->trks[track][tpt].y);
      dPoint p2(world->trks[track][tpt+1].x,world->trks[track][tpt+1].y);
      cnv.bck(p1); cnv.bck(p2);
      dPoint v1 = pscal(dPoint(pt)-p1, p2-p1)/pdist(p2,p1);
      if ((pdist(v1) < 0)||(pdist(v1)>pdist(p2-p1))) continue;
      if (pdist(dPoint(pt)-p1, v1) < radius){
        return make_pair(track, tpt);
      }
    }
  }
  return make_pair(-1,-1);
}

geo_data *
LayerTRK::get_world() const{
  return world;
}

iRect
LayerTRK::range() const{
  return myrange;
}
