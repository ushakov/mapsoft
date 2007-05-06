#ifndef LAYER_GEODATA_H
#define LAYER_GEODATA_H

#include <vector>
#include <sstream>
#include <fstream>
#include <math.h>

#include <assert.h>

#include "layer_geo.h"
#include "../geo_io/geo_convs.h"

#include "../geo_io/io.h"
#include "../utils/image_brez.h"


// Слой для показа точек и треков

class LayerGeoData : public LayerGeo {
private:
    geo_data * world; // указатель на геоданные
    convs::map2pt cnv; 
    g_map mymap;
    Rect<int> myrange;

public:

    LayerGeoData (geo_data * _world) : 
	world(_world), mymap(convs::mymap(*world)), 
        cnv(convs::mymap(*world), Datum("wgs84"), Proj("lonlat"), Options()) {
      recalc_range();
    }

    // получить/установить привязку layer'a
    virtual g_map get_ref() const {return mymap;}
    virtual void set_ref(const g_map & map){
      mymap=map; cnv = convs::map2pt(mymap, Datum("wgs84"), Proj("lonlat"), Options());
      recalc_range();
    }
    virtual void set_ref(){set_ref(convs::mymap(*world));}

    void recalc_range(){
      Rect<double> wgs_r = world->range_geodata();
      g_point p[4] = {
        g_point(wgs_r.x,wgs_r.y),
        g_point(wgs_r.x+wgs_r.w,wgs_r.y),
        g_point(wgs_r.x+wgs_r.w,wgs_r.y+wgs_r.h),
        g_point(wgs_r.x,wgs_r.y+wgs_r.h)};
      double minx(1e99), miny(1e99), maxx(-1e99), maxy(-1e99);
      for (int i=0; i<4; i++){
        std::cerr << p[i] << " -> ";
        cnv.bck(p[i]);
        std::cerr << p[i] << "\n";
        if (p[i].x<minx) minx=p[i].x;
        if (p[i].y<miny) miny=p[i].y;
        if (p[i].x>maxx) maxx=p[i].x;
        if (p[i].y>maxy) maxy=p[i].y;
      }

      if ((minx<maxx)&&(miny<maxy)) myrange = Rect<int>(int(minx), int(miny), int(maxx-minx+1), int(maxy-miny+1));
      else myrange = Rect<int>(0,0,0,0);
    }
      
    
    virtual void draw (Rect<int> src_rect, Image<int> & dst_img, Rect<int> dst_rect){
      clip_rects_for_image_loader(range(), src_rect, dst_img.range(), dst_rect);
      if (src_rect.empty() || dst_rect.empty()) return;

#ifdef DEBUG_LAYER_GEODATA
      std::cerr  << "LayerGeoData: draw " << src_rect << " -> "
               << dst_rect << " at " << dst_img <<  "\n";
#endif

      int c1 = 0xFF0000FF;
      int c2 = 0xFFFF0000;

      for (std::vector<g_track>::const_iterator it = world->trks.begin();
                                         it!= world->trks.end(); it++){
        int xo=0, yo=0;
        for (std::vector<g_trackpoint>::const_iterator pt = it->begin();
                                            pt!= it->end(); pt++){
          g_point p(pt->x,pt->y); cnv.bck(p);

          int x = int(dst_rect.x+((p.x-src_rect.x)*dst_rect.w)/src_rect.w);
          int y = int(dst_rect.y+((p.y-src_rect.y)*dst_rect.h)/src_rect.h);

	  // ideally, we should draw a line (xo, yo) -- (x, y)
	  // but part of the line can be outside the tile

          if (point_in_rect(Point<int>(int(p.x),int(p.y)), src_rect)){

            if (!pt->start) {
              image_brez::line(dst_img, xo,yo, x, y, 3, c2);
              image_brez::circ(dst_img, xo,yo, 2, 1, c1);
            } else {
              image_brez::circ(dst_img, x,y,2,1, c1);
            }
          }
          xo=x; yo=y;
        }
      }

      for (std::vector<g_waypoint_list>::const_iterator it = world->wpts.begin();
                                         it!= world->wpts.end(); it++){
        for (std::vector<g_waypoint>::const_iterator pt = it->begin();
                                            pt!= it->end(); pt++){
          g_point p(pt->x,pt->y); cnv.bck(p);

          int x = int(dst_rect.x+((p.x-src_rect.x)*dst_rect.w)/src_rect.w);
          int y = int(dst_rect.y+((p.y-src_rect.y)*dst_rect.h)/src_rect.h);

          if (point_in_rect(Point<int>(int(p.x),int(p.y)), src_rect)){
              image_brez::circ(dst_img, x,y,4,2, c2);
          }
        }
      }

    }

    g_waypoint * find_waypoint (Point<int> p) {
	Rect<int> target_rect (p - Point<int>(5,5), p + Point<int>(5,5));
	for (std::vector<g_waypoint_list>::iterator it = world->wpts.begin();
	     it!= world->wpts.end(); it++){
	    for (std::vector<g_waypoint>::iterator pt = it->begin();
		 pt!= it->end(); pt++){
		g_point wp(pt->x,pt->y); cnv.bck(wp);

		if (point_in_rect(Point<int>(int(wp.x),int(wp.y)), target_rect)){
		    return &(*pt);
		}
	    }
	}
	return 0;
    }

    void action_move (ActionData * ad) {
	if (ad->items.size() == 1) {
	    g_waypoint * wpt = find_waypoint (ad->items[0].p);
	    if (wpt) {
		// Add rubber tail etc...
		return;
	    }
	    ad->clear();
	    return;
	} else {
	    assert(ad->items.size() == 2);
	    g_waypoint * wpt = find_waypoint (ad->items[0].p);
	    assert(wpt);
	    wpt->x = ad->items[1].p.x;	
	    wpt->y = ad->items[1].p.y;
	    cnv.frw(*wpt);
	    ad->clear();
	    return;
	}
    }

    enum Actions { A_Move, A_Modify, A_Delete, A_ActionsNumber };

    virtual void do_action (ActionData * ad) {
	if (ad->items.size() == 0) return;   // Nothing to do!
	// check that all actions have the same type
	for (int i = 1; i < ad->items.size(); ++i) {
	    assert(ad->items[i].type == ad->items[0].type);
	}
	switch (ad->items[0].type) {
	case A_Move:
	    action_move(ad);
	    return;
	case A_Modify: // fallthrough
	case A_Delete:
	    // not yet implemented!
	    ad->clear();
	    return;
	default:
	    assert (!"Wrong type of action!");
	}
    }

    virtual std::vector<std::string> action_names () {
	std::vector<std::string> names;
	names.resize(A_ActionsNumber);
	names[A_Move] = "Move Waypoint";
	names[A_Modify] = "Modify Waypoint";
	names[A_Delete] = "Delete Waypoint";
	return names;
    }


    virtual Rect<int> range (){ return myrange;}
    
};


#endif 
