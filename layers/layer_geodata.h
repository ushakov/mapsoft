#ifndef LAYER_GEODATA_H
#define LAYER_GEODATA_H

#include <vector>
#include <sstream>
#include <fstream>
#include <math.h>

#include "layer_geo.h"
#include "../geo_io/geo_convs.h"

#include "../geo_io/io.h"
#include "../utils/image_brez.h"


// Слой для показа точек и треков

class LayerGeoData : public LayerGeo {
private:
  const geo_data * world; // указатель на геоданные
  convs::map2pt cnv; 
  g_map mymap;
  Rect<int> myrange;

public:

    LayerGeoData (const geo_data * _world) : 
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


    virtual Rect<int> range (){ return myrange;}
    
};


#endif 
