#ifndef LAYER_GEODATA1_H
#define LAYER_GEODATA1_H

#include "layer.h"
#include <math.h>

// Загрузка геоданных в координатах какого-то map-файла
// Кладем их на снимки google
// 

#include "../utils/mapsoft_convs.h"
#include "../utils/image_brez.h"

class LayerGeodata1 : public Layer {
private:
    const geo_data & world;
    convs::map2pt map_conv;
public:
    LayerGeodata1 (const g_map & map, const geo_data & _world) : 
      world(_world), map_conv(map, Datum("WGS84"), Proj("lonlat"), Options()){ }
    
    void draw (Rect<int> src, Image<int> & img, Rect<int> dst){    

      clip_rects_for_image_loader(src, src, img.range(), dst);
      if (src.empty() || dst.empty()) return;

	int c1 = 0xFF0000FF;
	int c2 = 0xFF00FFFF;

        for (std::vector<g_track>::const_iterator it = world.trks.begin();
                                         it!= world.trks.end(); it++){
        int xo=0, yo=0;
      
        for (std::vector<g_trackpoint>::const_iterator pt = it->points.begin();
                                            pt!= it->points.end(); pt++){
          g_point p(pt->x,pt->y);
          map_conv.bck(p);

          int x = (int)(dst.x+((p.x-src.x)*dst.w)/src.w);
          int y = (int)(dst.y+((p.y-src.y)*dst.h)/src.h);

          if (point_in_rect(Point<int>(x,y), dst)){
      
            if (!pt->start) {
              image_brez::line(img, xo,yo, x, y, 1, c2);
              image_brez::circ(img, x,y, 2, 1, c1);
            } else {
              image_brez::circ(img, x,y,2,1, c1);
            }
	  }
          xo=x; yo=y;
        }
      }
    }
        
    Rect<int> range(){
    	return Rect<int>(0,0,0,0);
    }

};


#endif /* LAYER_GEODATA1_H */
