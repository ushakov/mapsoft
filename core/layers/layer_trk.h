#ifndef LAYER_TRK_H
#define LAYER_TRK_H

#include <vector>
#include <sstream>
#include <fstream>
#include <math.h>

#include <assert.h>

#include "layer_geo.h"
#include "../libgeo/geo_convs.h"
#include "../utils/image_gd.h"

#include "../lib2d/point_utils.h"

#include <boost/shared_ptr.hpp>

//#define DEBUG_LAYER_TRK

/// Растровый слой для показа точек и треков

class LayerTRK
#ifndef SWIG
  : public LayerGeo
#endif  // SWIG
{
private:
    geo_data * world; // указатель на геоданные
    convs::map2pt cnv;
    g_map mymap;
    iRect myrange;
    int track_width_override;
    Color track_color_override;

public:

    LayerTRK (geo_data * _world) : 
      world(_world), mymap(convs::mymap(*world)), 
      cnv(convs::mymap(*world), Datum("wgs84"), Proj("lonlat")),
      myrange(rect_pump(cnv.bb_bck(world->range_geodata()), 1.0))
    { 
#ifdef DEBUG_LAYER_TRK
      std::cerr  << "LayerTRK: set_ref range: " << myrange << "\n";
#endif
      track_width_override = 0;
      track_color_override.value = 0;
    }

    void refresh(){
	myrange = rect_pump(cnv.bb_bck(world->range_geodata()), 1.0);
    }

    // получить/установить привязку layer'a
    virtual g_map get_ref() const {return mymap;}
    virtual void set_ref(const g_map & map){
      mymap=map; cnv = convs::map2pt(mymap, Datum("wgs84"), Proj("lonlat"));
      myrange=cnv.bb_bck(world->range_geodata(), 1.0);
#ifdef DEBUG_LAYER_TRK
      std::cerr  << "LayerTRK: set_ref range: " << myrange << "\n";
#endif

    }
    virtual void set_ref(){set_ref(convs::mymap(*world));}

    virtual Options get_config() {
	Options opt;
	opt.put("Track line width override", track_width_override);
	opt.put("Track color override", track_color_override);
	return opt;
    }

    /// Gets layer configuration from Options
    /// Default implementation does nothing
    virtual void set_config(const Options& opt) {
	std::cout << "LayerTRK: set_config" << opt << "\n";
	track_width_override = opt.get("Track line width override", track_width_override);
	track_color_override = opt.get("Track color override", track_color_override);
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
#ifdef DEBUG_LAYER_TRK
      std::cerr  << "LayerTRK: draw " << src_rect <<  " my: " << myrange << "\n";
#endif
      if (rect_intersect(myrange, rect_pump(src_rect,110)).empty()) return;
      boost::shared_ptr<ImageDrawContext> ctx(ImageDrawContext::Create(&image));

      for (std::vector<g_track>::const_iterator it = world->trks.begin();
                                         it!= world->trks.end(); it++){
	bool start=true;
	iPoint pi, pio;

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
	  pi = iPoint(p)-origin;

	  iRect line_bb(pio, pi); 

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
      ctx->StampAndClear();
    }


    std::pair<int, int> find_trackpoint (iPoint pt, int radius = 3) {
	iRect target_rect (pt,pt);
	target_rect = rect_pump(target_rect, radius);
	for (int track = 0; track < world->trks.size(); ++track) {
	    for (int tpt = 0; tpt < world->trks[track].size(); ++tpt) {
		g_point p(world->trks[track][tpt].x,world->trks[track][tpt].y);
		cnv.bck(p);

		if (point_in_rect(iPoint(p), target_rect)){
		    return std::make_pair(track, tpt);
		}
	    }
	}
	return std::make_pair(-1,-1);
    }

    // поиск трека. Находится сегмент, в которые тыкают, возвращается 
    // первая точка сегмента (0..size-2).
    // если тыкают в первую точку - возвращается -2, если в последнюю -- -3.
    std::pair<int, int> find_track (iPoint pt, int radius = 3) {
	iRect target_rect (pt,pt);
	target_rect = rect_pump(target_rect, radius);

	for (int track = 0; track < world->trks.size(); ++track) {
	    int ts = world->trks[track].size();
   	    if (ts>0){
		g_point p(world->trks[track][0].x,world->trks[track][0].y);
		cnv.bck(p);
		if (point_in_rect(iPoint(p), target_rect)){
		    return std::make_pair(track, -2);
		}
		p = g_point(world->trks[track][ts-1].x,world->trks[track][ts-1].y);
		cnv.bck(p);
		if (point_in_rect(iPoint(p), target_rect)){
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

    geo_data * get_world() { return world; }
    virtual iRect range (){ return myrange;}
};


#endif 
