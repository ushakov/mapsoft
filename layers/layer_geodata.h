#ifndef LAYER_GEODATA_H
#define LAYER_GEODATA_H

#include <vector>
#include <sstream>
#include <fstream>
#include <math.h>

#include <assert.h>

#include "layer_geo.h"
#include "../libgeo/geo_convs.h"
//#include "../libgeo_io/io.h"
//#include "../utils/image_brez.h"
#include "../utils/image_gd.h"

#include "../lib2d/point_utils.h"

#define DEBUG_LAYER_GEODATA

/// Растровый слой для показа точек и треков

class LayerGeoData : public LayerGeo {
private:
    geo_data * world; // указатель на геоданные
    convs::map2pt cnv; 
    g_map mymap;
    Rect<int> myrange;
    int track_width_override;
    Color track_color_override;
    Color wpt_color_override;
    Color wpt_bgcolor_override;
    int dot_width;

public:

    LayerGeoData (geo_data * _world) : 
      world(_world), mymap(convs::mymap(*world)), 
      cnv(convs::mymap(*world), Datum("wgs84"), Proj("lonlat")),
      myrange(rect_pump(cnv.bb_bck(world->range_geodata()), 110))
    { 
#ifdef DEBUG_LAYER_GEODATA
      std::cerr  << "LayerGeoData: set_ref range: " << myrange << "\n";
#endif
      track_width_override = 0;
      track_color_override.value = 0;
      wpt_color_override.value = 0;
      wpt_bgcolor_override.value = 0;
      dot_width = 8;
    }

    void refresh(){
	myrange = rect_pump(cnv.bb_bck(world->range_geodata()), 110);
    }

    // получить/установить привязку layer'a
    virtual g_map get_ref() const {return mymap;}
    virtual void set_ref(const g_map & map){
      mymap=map; cnv = convs::map2pt(mymap, Datum("wgs84"), Proj("lonlat"));
      myrange=cnv.bb_bck(world->range_geodata());
#ifdef DEBUG_LAYER_GEODATA
      std::cerr  << "LayerGeoData: set_ref range: " << myrange << "\n";
#endif

    }
    virtual void set_ref(){set_ref(convs::mymap(*world));}

    virtual Options get_config() {
	Options opt;
	opt.put("Track line width override", track_width_override);
	opt.put("Track color override", track_color_override);
	opt.put("Waypoint color override", wpt_color_override);
	opt.put("Waypoint background color override", wpt_bgcolor_override);
	opt.put("Waypoint dot size", dot_width);
	return opt;
    }

    /// Gets layer configuration from Options
    /// Default implementation does nothing
    virtual void set_config(const Options& opt) {
	std::cout << "LayerGeoData: set_config" << opt << "\n";
	opt.get("Track line width override", track_width_override);
	opt.get("Track color override", track_color_override);
	opt.get("Waypoint color override", wpt_color_override);
	opt.get("Waypoint background color override", wpt_bgcolor_override);
	opt.get("Waypoint dot size", dot_width);
    }

    
    // Optimized get_image to return empty image outside of bounds.
    virtual Image<int> get_image (Rect<int> src){
	if (rect_intersect(myrange, src).empty()) {
	    return Image<int>(0,0);
	}
	Image<int> ret(src.w, src.h, 0);
	draw(src.TLC(), ret);
	return ret;
    }

    virtual void draw(const Point<int> origin, Image<int> & image){
      Rect<int> src_rect = image.range() + origin;
#ifdef DEBUG_LAYER_GEODATA
      std::cerr  << "LayerGeoData: draw " << src_rect <<  " my: " << myrange << "\n";
#endif
      if (rect_intersect(myrange, rect_pump(src_rect,110)).empty()) return;
      boost::shared_ptr<ImageDrawContext> ctx(ImageDrawContext::Create(&image));

      for (std::vector<g_track>::const_iterator it = world->trks.begin();
                                         it!= world->trks.end(); it++){
	bool start=true;
	Point<int> pi, pio;

	int w = it->width;
	if (track_width_override != 0) {
	    w = track_width_override;
	}

	Color color = it->color;
	if ((track_color_override.value & 0xff000000) != 0) {
	    color = track_color_override;
	}
        for (std::vector<g_trackpoint>::const_iterator pt = it->begin();
                                            pt!= it->end(); pt++){
          g_point p(pt->x,pt->y); cnv.bck(p);
	  pi = Point<int>(p)-origin;

	  Rect<int> line_bb(pio, pi); 

	  line_bb = rect_pump(line_bb, 2*w);
	  if (!rect_intersect(line_bb, image.range()).empty()) {
	      if (!start){
	        if (!pt->start) ctx->DrawLine(pio, pi, w, color.value);
	        ctx->DrawCircle(pio, w, 2, color.value, false);
	      }
	      else start=false;
	  }
	  pio=pi;
        }
        ctx->DrawCircle(pio, w, 2, color.value, false);
      }

      Rect<int> rect_pumped = rect_pump(image.range(), 6);

      for (std::vector<g_waypoint_list>::const_iterator it = world->wpts.begin();
	   it!= world->wpts.end(); it++){
	Point<int> pi, pio;
        for (std::vector<g_waypoint>::const_iterator pt = it->begin();
                                            pt!= it->end(); pt++){
          g_point p(pt->x,pt->y); cnv.bck(p);
	  pi = Point<int>(p)-origin;

	  Color color = pt->color;
	  if ((wpt_color_override.value & 0xff000000) != 0) {
	      color = wpt_color_override;
	  }
	  Color bgcolor = pt->bgcolor;
	  if ((wpt_bgcolor_override.value & 0xff000000) != 0) {
	      bgcolor = wpt_bgcolor_override;
	  }
	
          if (point_in_rect(pi, rect_pumped)){
	      ctx->DrawCircle(pi, dot_width, 1, color.value, true, bgcolor.value);
	  }
	  Rect<int> textbb = ImageDrawContext::GetTextMetrics(pt->name.c_str());
	  Rect<int> padded = rect_pump(textbb, 2);
	  Point<int> shift = Point<int>(2,-10);
	  Point<int> shifted = pi + shift;
	  if (point_in_rect(pi, rect_pump (image.range(), padded+shift))) {
	      ctx->DrawLine(pi, (padded + shifted).TLC(), 1, color.value);
	      ctx->DrawFilledRect(padded + shifted, bgcolor.value);
	      ctx->DrawRect(padded + shifted, 1, color.value);
	      ctx->DrawText(shifted.x, shifted.y, color.value, pt->name.c_str());
	  }
          pio=pi;
        }
      }
      ctx->StampAndClear();
    }

    geo_data * get_world() { return world; }

    std::pair<int, int> find_waypoint (Point<int> pt, int radius = 3) {
	Rect<int> target_rect (pt,pt);
	target_rect = rect_pump(target_rect, radius);
	for (int wptl = 0; wptl < world->wpts.size(); ++wptl) {
	    for (int wpt = 0; wpt < world->wpts[wptl].size(); ++wpt) {
		g_point p(world->wpts[wptl][wpt].x,world->wpts[wptl][wpt].y);
		cnv.bck(p);
//		std::cout << "wpt: (" << wptl << "," << wpt << ")[" << world->wpts[wptl][wpt].name << "] @ " << wp << std::endl;

		if (point_in_rect(Point<int>(p), target_rect)){
		    return std::make_pair(wptl, wpt);
		}
	    }
	}
	return std::make_pair(-1,-1);
    }
    
    std::pair<int, int> find_trackpoint (Point<int> pt, int radius = 3) {
	Rect<int> target_rect (pt,pt);
	target_rect = rect_pump(target_rect, radius);
	for (int track = 0; track < world->trks.size(); ++track) {
	    for (int tpt = 0; tpt < world->trks[track].size(); ++tpt) {
		g_point p(world->trks[track][tpt].x,world->trks[track][tpt].y);
		cnv.bck(p);

		if (point_in_rect(Point<int>(p), target_rect)){
		    return std::make_pair(track, tpt);
		}
	    }
	}
	return std::make_pair(-1,-1);
    }

    // поиск трека. Находится сегмент, в которые тыкают, возвращается 
    // первая точка сегмента (0..size-2).
    // если тыкают в первую точку - возвращается -2, если в последнюю -- -3.
    std::pair<int, int> find_track (Point<int> pt, int radius = 3) {
	Rect<int> target_rect (pt,pt);
	target_rect = rect_pump(target_rect, radius);

	for (int track = 0; track < world->trks.size(); ++track) {
	    int ts = world->trks[track].size();
   	    if (ts>0){
		g_point p(world->trks[track][0].x,world->trks[track][0].y);
		cnv.bck(p);
		if (point_in_rect(Point<int>(p), target_rect)){
		    return std::make_pair(track, -2);
		}
		p = g_point(world->trks[track][ts-1].x,world->trks[track][ts-1].y);
		cnv.bck(p);
		if (point_in_rect(Point<int>(p), target_rect)){
		    return std::make_pair(track, -3);
		}
            }
	    for (int tpt = 0; tpt < ts-1; ++tpt) {
		g_point p1(world->trks[track][tpt].x,world->trks[track][tpt].y);
		g_point p2(world->trks[track][tpt+1].x,world->trks[track][tpt+1].y);
		cnv.bck(p1); cnv.bck(p2);

		g_point v1 = pscal(g_point(pt)-p1, p2-p1)/pdist(p2,p1);
		if ((pdist(v1) < 0)||(pdist(v1)>pdist(p2-p1))) continue;
                if (pdist(g_point(pt)-p1, v1) < radius){
		    return std::make_pair(track, tpt);
		}

	    }
	}
	return std::make_pair(-1,-1);
    }

    virtual Rect<int> range (){ return myrange;}
    
};


#endif 
