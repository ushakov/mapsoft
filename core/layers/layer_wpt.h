#ifndef LAYER_WPT_H
#define LAYER_WPT_H

#include <vector>
#include <sstream>
#include <fstream>
#include <math.h>

#include <assert.h>

#include "layer_geo.h"
#include "geo/geo_convs.h"
#include "utils/cairo_wrapper.h"

#include "2d/point.h"

#include <boost/shared_ptr.hpp>

//#define DEBUG_LAYER_WPT

/// Растровый слой для показа точек и треков

class LayerWPT
#ifndef SWIG
  : public LayerGeo
#endif  // SWIG
{
private:
    geo_data * world; // указатель на геоданные
    convs::map2pt cnv; 
    g_map mymap;
    iRect myrange;
    Color wpt_color_override;
    Color wpt_bgcolor_override;
    int dot_width;

public:

    LayerWPT (geo_data * _world) : 
      world(_world), mymap(convs::mymap(*world)), 
      cnv(convs::mymap(*world), Datum("wgs84"), Proj("lonlat")),
      myrange(rect_pump(cnv.bb_bck(world->range_geodata()), 1.0))
    { 
#ifdef DEBUG_LAYER_WPT
      std::cerr  << "LayerWPT: set_ref range: " << myrange << "\n";
#endif
      wpt_color_override.value = 0;
      wpt_bgcolor_override.value = 0;
      dot_width = 4;
    }

    void refresh(){
	myrange = rect_pump(cnv.bb_bck(world->range_geodata()), 1.0);
    }

    // получить/установить привязку layer'a
    virtual g_map get_ref() const {return mymap;}
    virtual void set_ref(const g_map & map){
      mymap=map; cnv = convs::map2pt(mymap, Datum("wgs84"), Proj("lonlat"));
      myrange=cnv.bb_bck(world->range_geodata(), 1.0);
#ifdef DEBUG_LAYER_WPT
      std::cerr  << "LayerWPT: set_ref range: " << myrange << "\n";
#endif

    }
    virtual void set_ref(){set_ref(convs::mymap(*world));}

    virtual Options get_config() {
	Options opt;
	opt.put("Waypoint color override", wpt_color_override);
	opt.put("Waypoint background color override", wpt_bgcolor_override);
	opt.put("Waypoint dot size", dot_width);
	return opt;
    }

    /// Gets layer configuration from Options
    /// Default implementation does nothing
    virtual void set_config(const Options& opt) {
	std::cout << "LayerWPT: set_config" << opt << "\n";
	wpt_color_override = opt.get("Waypoint color override", wpt_color_override);
	wpt_bgcolor_override = opt.get("Waypoint background color override", wpt_bgcolor_override);
	dot_width = opt.get("Waypoint dot size", dot_width);
    }


    // Optimized get_image to return empty image outside of bounds.
    virtual iImage get_image (iRect src){
	if (rect_intersect(myrange, src).empty()) {
	    return iImage(0,0);
	}
	iImage ret(src.w, src.h, 0);
	draw(src.TLC(), ret);
	return ret;
    }

    virtual void draw(const iPoint origin, iImage & image){
      iRect src_rect = image.range() + origin;
#ifdef DEBUG_LAYER_WPT
      std::cerr  << "LayerWPT: draw " << src_rect <<  " my: " << myrange << "\n";
#endif
      if (rect_intersect(myrange, rect_pump(src_rect,110)).empty()) return;
      CairoWrapper cr(image);

      double text_pad=2.0;
      dPoint flag_shift(0,-10);
      dPoint text_shift = flag_shift + dPoint(text_pad, -text_pad);

      for (std::vector<g_waypoint_list>::const_iterator it = world->wpts.begin();
	   it!= world->wpts.end(); it++){


        for (std::vector<g_waypoint>::const_iterator pt = it->begin();
                                            pt!= it->end(); pt++){

          dPoint p(pt->x,pt->y); cnv.bck(p); p-=origin;

	  Color color = pt->color;
	  if ((wpt_color_override.value & 0xff000000) != 0) {
	      color = wpt_color_override;
	  }
	  Color bgcolor = pt->bgcolor;
	  if ((wpt_bgcolor_override.value & 0xff000000) != 0) {
	      bgcolor = wpt_bgcolor_override;
	  }

          cr->move_to(p + text_shift);
          dRect txt_rect = cr->get_text_extents(pt->name);
          txt_rect = rect_pump(txt_rect, text_pad);
          dRect pt_rect = rect_pump(txt_rect, p);
          pt_rect = rect_pump(pt_rect, double(dot_width+1));

          if (!rect_intersect(pt_rect, dRect(image.range())).empty()){
            /*
            // debug
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


    std::pair<int, int> find_waypoint (iPoint pt, int radius = 3) {
	iRect target_rect (pt,pt);
	target_rect = rect_pump(target_rect, radius);
	for (int wptl = 0; wptl < world->wpts.size(); ++wptl) {
	    for (int wpt = 0; wpt < world->wpts[wptl].size(); ++wpt) {
		dPoint p(world->wpts[wptl][wpt].x,world->wpts[wptl][wpt].y);
		cnv.bck(p);
//		std::cout << "wpt: (" << wptl << "," << wpt << ")[" << world->wpts[wptl][wpt].name << "] @ " << wp << std::endl;

		if (point_in_rect(iPoint(p), target_rect)){
		    return std::make_pair(wptl, wpt);
		}
	    }
	}
	return std::make_pair(-1,-1);
    }

    geo_data * get_world() { return world; }
    virtual iRect range (){ return myrange;}
};


#endif
