#ifndef LAYER_GEODATA_H
#define LAYER_GEODATA_H

#include "layer.h"
#include <math.h>

// Загрузка геоданных в координатах google
// Кладем их на снимки google
// 

#include "../geo_io/geo_data.h"
#include "../utils/image_google_misc.h"
#include "../utils/image_brez.h"

class LayerGeodata : public Layer {
public:
    LayerGeodata (const geo_data & _world, int _gscale) : world(_world){ 
      set_scale(_gscale);
    }
    
    virtual void draw (Rect<int> src, Image<int> & img, Rect<int> dst){    

      clip_rects_for_image_loader(data_range, src, img.range(), dst);
      if (src.empty() || dst.empty()) return;

      int c1 = 0xFF0000FF;
      int c2 = 0xFF00FFFF;

      for (std::vector<g_track>::const_iterator it = world.trks.begin();
                                         it!= world.trks.end(); it++){
        int xo=0, yo=0;
      
        for (std::vector<g_trackpoint>::const_iterator pt = it->begin();
                                            pt!= it->end(); pt++){
          Point<int> p = google::lonlat2xy(gscale, Point<double>(pt->x,pt->y));

          int x = dst.x+((p.x-src.x)*dst.w)/src.w;
          int y = dst.y+((p.y-src.y)*dst.h)/src.h;

          if (point_in_rect(p, src)){
      
            if (!pt->start) {
              image_brez::line(img, xo,yo, x, y, 1, c2);
              image_brez::circ(img, xo,yo, 2, 1, c1);
            } else {
              image_brez::circ(img, x,y,2,1, c1);
            }
	  }
          xo=x; yo=y;
        }
      }
    }
        
        virtual Rect<int> range (){
        	return data_range;
    }

    void set_scale(int scale){
      gscale = scale;
      int xmin =  0xFFFFFFF, ymin =  0xFFFFFFF;
      int xmax = -0xFFFFFFF, ymax = -0xFFFFFFF;
      for (std::vector<g_track>::const_iterator it = world.trks.begin();
                                   it!= world.trks.end(); it++){
        for (std::vector<g_trackpoint>::const_iterator pt = it->begin();
                                          pt!= it->end(); pt++){
          Point<int> p = google::lonlat2xy(gscale, Point<double>(pt->x,pt->y));
	
          if (xmin>p.x) xmin=p.x;
          if (xmax<p.x) xmax=p.x;
          if (ymin>p.y) ymin=p.y;
          if (ymax<p.y) ymax=p.y;
        } 
      }
      if ((xmin>xmax)||(ymin>ymax)) data_range = Rect<int>(0,0,0,0);
      else data_range = Rect<int>(xmin,ymin, xmax-xmin+1, ymax-ymin+1);
      std::cerr << "geodata layer: " << data_range <<"\n";
    }

private:
    int gscale;
    const geo_data & world;
    Rect<int> data_range;
};


#endif /* LAYER_GEODATA_H */
