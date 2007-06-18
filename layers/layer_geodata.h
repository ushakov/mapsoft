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

    static const int wpt_radius = 3;
    static const int tpt_radius = 2;
 
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
      boost::shared_ptr<ImageDrawContext> ctx(ImageDrawContext::Create(&dst_img));

      int c_r = 0xFF0000FF;
      int c_g = 0xFF00FF00;
      int c_b = 0xFFFF0000;
      int c_y = 0xFF00FFFF;
      int c_m = 0xFFFF00FF;
      int c_c = 0xFFFFFF00;
      int c_bl = 0xFF000000;

      for (std::vector<g_track>::const_iterator it = world->trks.begin();
                                         it!= world->trks.end(); it++){
	
        int xo=0, yo=0;
        for (std::vector<g_trackpoint>::const_iterator pt = it->begin();
                                            pt!= it->end(); pt++){
          g_point p(pt->x,pt->y); cnv.bck(p);

          int x = int(dst_rect.x+((p.x-src_rect.x)*dst_rect.w)/src_rect.w);
          int y = int(dst_rect.y+((p.y-src_rect.y)*dst_rect.h)/src_rect.h);

	  Rect<int> line_bb(Point<int>(xo, yo), Point<int>(x,y));
	  line_bb = rect_pump(line_bb, 2);
	  if (!rect_intersect(line_bb, dst_rect).empty()) {
	      if (!pt->start) {
		  ctx->DrawLine(Point<int>(xo,yo), Point<int>(x, y), 3, c_b);
		  ctx->DrawFilledRect(Rect<int>(x-2,y-2,4,4), c_m);
	      } else {
		  ctx->DrawFilledRect(Rect<int>(x-2,y-2,4,4), c_m);
	      }
	  }
	  xo=x; yo=y;
        }
      }

      Rect<int> dst_rect_pumped = rect_pump(dst_rect, 6);

      for (std::vector<g_waypoint_list>::const_iterator it = world->wpts.begin();
	   it!= world->wpts.end(); it++){
        for (std::vector<g_waypoint>::const_iterator pt = it->begin();
                                            pt!= it->end(); pt++){
          g_point p(pt->x,pt->y); cnv.bck(p);

          int x = int(dst_rect.x+((p.x-src_rect.x)*dst_rect.w)/src_rect.w);
          int y = int(dst_rect.y+((p.y-src_rect.y)*dst_rect.h)/src_rect.h);
	
          if (point_in_rect(Point<int>(x, y), dst_rect_pumped)){
	      ctx->DrawFilledRect(Rect<int>(x-3,y-3,6,6), c_y);
	      ctx->DrawRect(Rect<int>(x-3,y-3,6,6), 1, c_b);
	  }
	  Rect<int> textbb = ImageDrawContext::GetTextMetrics(pt->name);
	  Rect<int> padded = rect_pump (textbb, 2);
	  Point<int> wpt(x,y);
	  Point<int> shift = Point<int>(2,-10);
	  Point<int> shifted = wpt + shift;
	  if (point_in_rect(wpt, rect_pump (dst_rect, padded+shift))) {
	      ctx->DrawLine(wpt, (padded + shifted).TLC(), 1, c_b);
	      ctx->DrawFilledRect(padded + shifted, c_y);
	      ctx->DrawRect(padded + shifted, 1, c_b);
	      ctx->DrawText(shifted.x, shifted.y, c_b, pt->name);
	  }
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

		if (point_in_rect(Point<int>(int(wp.x),int(wp.y)), target_rect)){
		    std::make_pair(wptl, wpt);
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
		    std::make_pair(track, tpt);
		}
	    }
	}
	return std::make_pair(-1,-1);
    }

    virtual Rect<int> range (){ return myrange;}
    
};


#endif 
