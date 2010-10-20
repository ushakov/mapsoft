#include <vector>
#include <cmath>

#include "layer_wpt.h"
#include "utils/cairo_wrapper.h"

using namespace std;

LayerWPT::LayerWPT (geo_data * _world) :
      world(_world), mymap(convs::mymap(*world)),
      cnv(convs::mymap(*world), Datum("wgs84"), Proj("lonlat")),
      myrange(rect_pump(cnv.bb_bck(world->range_geodata()), 1.0)){
#ifdef DEBUG_LAYER_WPT
  cerr  << "LayerWPT: set_ref range: " << myrange << "\n";
#endif
  dot_width = 4;
}

void
LayerWPT::refresh(){
  myrange = rect_pump(cnv.bb_bck(world->range_geodata()), 1.0);
}

g_map
LayerWPT::get_ref() const {
  return mymap;
}

void
LayerWPT::set_ref(const g_map & map){
  mymap=map; cnv = convs::map2pt(mymap, Datum("wgs84"), Proj("lonlat"));
  myrange=cnv.bb_bck(world->range_geodata(), 1.0);
#ifdef DEBUG_LAYER_WPT
  cerr  << "LayerWPT: set_ref range: " << myrange << "\n";
#endif
}

void
LayerWPT::set_ref(){
  set_ref(convs::mymap(*world));
}

iImage
LayerWPT::get_image (iRect src){
  if (rect_intersect(myrange, src).empty()) return iImage(0,0);
  iImage ret(src.w, src.h, 0);
  draw(src.TLC(), ret);
  return ret;
}

void
LayerWPT::draw(const iPoint origin, iImage & image){
  iRect src_rect = image.range() + origin;
#ifdef DEBUG_LAYER_WPT
  cerr  << "LayerWPT: draw " << src_rect <<  " my: " << myrange << "\n";
#endif
  if (rect_intersect(myrange, rect_pump(src_rect,110)).empty()) return;
  CairoWrapper cr(image);

  double text_pad=2.0;
  dPoint flag_shift(0,-10);
  dPoint text_shift = flag_shift + dPoint(text_pad, -text_pad);

  vector<g_waypoint_list>::const_iterator it;
  for (it=world->wpts.begin(); it!=world->wpts.end(); it++){
    vector<g_waypoint>::const_iterator pt;
    for (pt=it->begin(); pt!= it->end(); pt++){
      dPoint p(pt->x,pt->y); cnv.bck(p); p-=origin;

      Color color = pt->color;
      Color bgcolor = pt->bgcolor;

      cr->move_to(p + text_shift);
      dRect txt_rect = cr->get_text_extents(pt->name);
      txt_rect = rect_pump(txt_rect, text_pad);
      dRect pt_rect = rect_pump(txt_rect, p);
      pt_rect = rect_pump(pt_rect, double(dot_width+1));

      if (!rect_intersect(pt_rect, dRect(image.range())).empty()){
        /* // debug
        cr->rectangle(pt_rect);
        cr->set_color(0xFF00FF);
        cr->fill();
        */

        cr->move_to(p);
        cr->circle(p, dot_width);
        cr->rectangle(txt_rect);
        cr->set_color(bgcolor.value);
        cr->fill();

        cr->move_to(p);
        cr->circle(p, dot_width);
        cr->move_to(p+dPoint(0,-dot_width));
        cr->rel_line_to(flag_shift);
        cr->rectangle(txt_rect);
        cr->set_color(color.value);
        cr->set_line_width(1);
        cr->stroke();

        cr->set_color(color.value);
        cr->move_to(p + text_shift);
        cr->show_text(pt->name);
      }
    }
  }
}

pair<int, int>
LayerWPT::find_waypoint (iPoint pt, int radius) {
  iRect target_rect (pt,pt);
  target_rect = rect_pump(target_rect, radius);
  for (int wptl = 0; wptl < world->wpts.size(); ++wptl) {
    for (int wpt = 0; wpt < world->wpts[wptl].size(); ++wpt) {
      dPoint p(world->wpts[wptl][wpt].x,world->wpts[wptl][wpt].y);
      cnv.bck(p);
//	cout << "wpt: (" << wptl << "," << wpt << ")[" << world->wpts[wptl][wpt].name << "] @ " << wp << endl;

      if (point_in_rect(iPoint(p), target_rect)){
        return make_pair(wptl, wpt);
      }
    }
  }
  return make_pair(-1,-1);
}

geo_data *
LayerWPT::get_world() const{
  return world;
}

iRect
LayerWPT::range() const{
  return myrange;
}
