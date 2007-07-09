#ifndef LAYER_GEODATA_H
#define LAYER_GEODATA_H

#include <vector>
#include <sstream>
#include <fstream>
#include <math.h>

#include <assert.h>

#include <layers/layer_geo.h>
#include <geo_io/geo_convs.h>
#include <geo_io/io.h>
//#include <utils/image_brez.h>
#include <utils/image_draw.h>


// Слой для показа точек и треков

class LayerGeoData : public LayerGeo {
private:
    geo_data * world; // указатель на геоданные
    convs::map2pt cnv; 
    g_map mymap;
    Rect<int> myrange;

    static const int wpt_radius = 9;
    static const int tpt_radius = 2;
 
public:

    LayerGeoData (geo_data * _world) : 
      world(_world), mymap(convs::mymap(*world)), 
      cnv(convs::mymap(*world), Datum("wgs84"), Proj("lonlat"), Options()),
      myrange(cnv.bb_bck(world->range_geodata()))
    { 
#ifdef DEBUG_LAYER_GEODATA
      std::cerr  << "LayerGeoData: set_ref range: " << myrange << "\n";
#endif
    }

    void refresh(){myrange=cnv.bb_bck(world->range_geodata());}

    // получить/установить привязку layer'a
    virtual g_map get_ref() const {return mymap;}
    virtual void set_ref(const g_map & map){
      mymap=map; cnv = convs::map2pt(mymap, Datum("wgs84"), Proj("lonlat"), Options());
      myrange=cnv.bb_bck(world->range_geodata());
#ifdef DEBUG_LAYER_GEODATA
      std::cerr  << "LayerGeoData: set_ref range: " << myrange << "\n";
#endif

    }
    virtual void set_ref(){set_ref(convs::mymap(*world));}


    virtual void draw(const Point<int> origin, Image<int> & image){
      Rect<int> src_rect = image.range() + origin;
#ifdef DEBUG_LAYER_GEODATA
      std::cerr  << "LayerGeoData: draw " << src_rect <<  " my: " << myrange << "\n";
#endif
      if (rect_intersect(myrange, src_rect).empty()) return;
      boost::shared_ptr<ImageDrawContext> ctx(ImageDrawContext::Create(&image));

      for (std::vector<g_track>::const_iterator it = world->trks.begin();
                                         it!= world->trks.end(); it++){
	bool start=true;
	Point<int> pi, pio;
        for (std::vector<g_trackpoint>::const_iterator pt = it->begin();
                                            pt!= it->end(); pt++){
          g_point p(pt->x,pt->y); cnv.bck(p);
	  pi = Point<int>(p)-origin;

	  Rect<int> line_bb(pio, pi); 

	  line_bb = rect_pump(line_bb, 2);
	  if (!rect_intersect(line_bb, image.range()).empty()) {
	      if ((!pt->start)&&(!start)) {
		  ctx->DrawLine(pio, pi, 3, COLOR_BLUE);
		  ctx->DrawFilledRect(Rect<int>(-2,-2,4,4) + pi, COLOR_MAGENTA);
	      } else {
		  ctx->DrawFilledRect(Rect<int>(-2,-2,4,4) + pi, COLOR_MAGENTA);
                  start=false;
	      }
	  }
	  pio=pi;
        }
      }

      Rect<int> rect_pumped = rect_pump(image.range(), 6);

      for (std::vector<g_waypoint_list>::const_iterator it = world->wpts.begin();
	   it!= world->wpts.end(); it++){
	Point<int> pi, pio;
        for (std::vector<g_waypoint>::const_iterator pt = it->begin();
                                            pt!= it->end(); pt++){
          g_point p(pt->x,pt->y); cnv.bck(p);
	  pi = Point<int>(p)-origin;
	
          if (point_in_rect(pi, rect_pumped)){
	      ctx->DrawFilledRect(Rect<int>(-3,-3,6,6) + pi, COLOR_YELLOW);
	      ctx->DrawRect(Rect<int>(-3,-3,6,6) + pi, 1, COLOR_BLUE);
	  }
	  Rect<int> textbb = ImageDrawContext::GetTextMetrics(pt->name);
	  Rect<int> padded = rect_pump(textbb, 2);
	  Point<int> shift = Point<int>(2,-10);
	  Point<int> shifted = pi + shift;
	  if (point_in_rect(pi, rect_pump (image.range(), padded+shift))) {
	      ctx->DrawLine(pi, (padded + shifted).TLC(), 1, COLOR_BLUE);
	      ctx->DrawFilledRect(padded + shifted, COLOR_YELLOW);
	      ctx->DrawRect(padded + shifted, 1, COLOR_BLUE);
	      ctx->DrawText(shifted.x, shifted.y, COLOR_BLUE, pt->name);
	  }
          pio=pi;
        }
      }
      ctx->StampAndClear();
    }

    geo_data * get_world() { return world; }

    std::pair<int, int> find_waypoint (Point<int> p) {
	Rect<int> target_rect (p,p);
	target_rect = rect_pump(target_rect, wpt_radius);
	for (int wptl = 0; wptl < world->wpts.size(); ++wptl) {
	    for (int wpt = 0; wpt < world->wpts[wptl].size(); ++wpt) {
		g_point wp(world->wpts[wptl][wpt].x,world->wpts[wptl][wpt].y);
		cnv.bck(wp);
		std::cout << "wpt: (" << wptl << "," << wpt << ")[" << world->wpts[wptl][wpt].name << "] @ " << wp << std::endl;

		if (point_in_rect(Point<int>(int(wp.x),int(wp.y)), target_rect)){
		    return std::make_pair(wptl, wpt);
		}
	    }
	}
	return std::make_pair(-1,-1);
    }
    
    std::pair<int, int> find_trackpoint (Point<int> p) {
	Rect<int> target_rect (p,p);
	target_rect = rect_pump(target_rect, tpt_radius);
	for (int track = 0; track < world->trks.size(); ++track) {
	    for (int tpt = 0; tpt < world->trks[track].size(); ++tpt) {
		g_point wp(world->trks[track][tpt].x,world->trks[track][tpt].y);
		cnv.bck(wp);

		if (point_in_rect(Point<int>(int(wp.x),int(wp.y)), target_rect)){
		    return std::make_pair(track, tpt);
		}
	    }
	}
	return std::make_pair(-1,-1);
    }

    virtual Rect<int> range (){ return myrange;}
    
};


#endif 
