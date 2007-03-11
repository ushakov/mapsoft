#ifndef LAYER_GEOMAP_H
#define LAYER_GEOMAP_H

#include <vector>
#include <sstream>
#include <fstream>
#include <math.h>

#include "layer_geo.h"
#include "../geo_io/geo_convs.h"
#include "../utils/cache.h"
#include "../loaders/image_r.h"
#include "../loaders/image_i.h"

#include "../geo_io/io.h"


// Слой для показа привязанных карт.

class LayerGeoMap : public LayerGeo {
private:
  const geo_data *world;                   // привязки карт
  std::vector<convs::map2map> m2ms;  // преобразования из каждой карты в mymap
  std::vector<double> scales;        // во сколько раз мы сжимаем карты при загрузке
  std::vector<int>    iscales;       // во сколько раз мы сжимаем карты при загрузке
  Rect<int> map_range;               // габариты карты
  Cache<int, Image<int> > image_cache;    // кэш изображений
  Options O; // для всех карт должны быть одинаковы!
  g_map mymap;

public:

    LayerGeoMap (const geo_data *_world) : 
      world(_world), image_cache(4), mymap(convs::mymap(*world)){ }

    // получить/установить привязку layer'a
    g_map get_ref() const {return mymap;}
    void set_ref(const g_map & map){mymap=map; make_m2ms(); image_cache.clear();}
    void set_ref(){set_ref(convs::mymap(*world));}

    // построение преобразований, вычисление габаритов карты
    void make_m2ms(){
      if ((world == NULL)||(world->maps.size()==0)) return;

      m2ms.clear();
      scales.clear();
      iscales.clear();
      Point<int> brd_min(0xFFFFFF, 0xFFFFFF), brd_max(-0xFFFFFF,-0xFFFFFF);

      for (int i=0; i< world->maps.size(); i++){
        convs::map2map c(world->maps[i], mymap);
	m2ms.push_back(c);
        
        scales.push_back(world->maps[i].mpp());
        iscales.push_back(1);

        for (int j=0; j<c.border_dst.size(); j++){
	  g_point p = c.border_dst[j];
          if (brd_min.x > p.x) brd_min.x = int(p.x);
          if (brd_min.y > p.y) brd_min.y = int(p.y);
          if (brd_max.x < p.x) brd_max.x = int(p.x);
          if (brd_max.y < p.y) brd_max.y = int(p.y);
        }
      }
      map_range = Rect<int>(brd_min, brd_max);

#ifdef DEBUG_LAYER_MAP
      std::cerr << "LayerMap: Setting map conversions. Range: " << map_range << "\n";
#endif
    }
    
    virtual void draw (Rect<int> src_rect, Image<int> & dst_img, Rect<int> dst_rect){

        clip_rects_for_image_loader(map_range, src_rect, dst_img.range(), dst_rect);
        if (src_rect.empty() || dst_rect.empty()) return;

//#ifdef DEBUG_LAYER_MAP
//      std::cerr  << "LayerMap: draw " << src_rect << " -> " 
//		 << dst_rect << " at " << dst_img <<  "\n";
//#endif
	double sc_x = src_rect.w/dst_rect.w;
	double sc_y = src_rect.h/dst_rect.h;

        if ((world == NULL)||(world->maps.size()==0)) return;
	for (int i=0; i<world->maps.size(); i++){

          std::string file = world->maps[i].file;

          if (!m2ms[i].tst_frw.test_range(src_rect)){  
#ifdef DEBUG_LAYER_MAP
            std::cerr  << "LayerMap: Skipping Image " << file << "\n";
#endif
	        
	    continue;
	  }
          if (!image_cache.contains(i)){

            int scale = int((0.01+scales[i]) * (sc_x<sc_y? sc_x:sc_y));
            if (scale <=0) scale = 1;


#ifdef DEBUG_LAYER_MAP
      std::cerr  << "LayerMap: Loading Image " << file
		 << " at scale " << scale 
		 << " scales[i]: " << scales[i] 
		 << " sc_x: " << sc_x  
	         << " sc_y: " << sc_y << "\n";
#endif


            image_cache.add(i, image_r::load(file.c_str(), scale));
	    iscales[i] = scale;
          }
	  Image<int> im = image_cache.get(i);

          m2ms[i].image_frw(im, iscales[i], src_rect, dst_img, dst_rect);
        }
    }

    virtual Rect<int> range (){
	return map_range;
    }
    
    // полезная функция, чтобы смотреть, как выглядят границы исходных карт на новой карте
    void dump_maps(char *file){
      std::ofstream f(file);
      f<< "#FIG 3.2\n"
       << "Portrait\n"
       << "Center\n"
       << "Metric\n"
       << "A4\n"
       << "100.0\n"
       << "Single\n"
       << "-2\n"
       << "1200 2\n";
      for (int i=0;i<m2ms.size();i++){
        int bs = m2ms[i].border_dst.size();
        f << "2 3 0 1 4 -1 52 -1 -1 0.000 0 0 -1 0 0 "
          << bs << "\n\t";
	for (int j=0; j<bs; j++){
          f << " " << int(m2ms[i].border_dst[j].x) 
            << " " << int(m2ms[i].border_dst[j].y);
        }
        f << "\n";
        if (bs==0) continue;
        f << "4 0 4 50 -1 18 20 0.0000 4 225 630 " 
          << int(m2ms[i].border_dst[0].x+100) << " " 
          << int(m2ms[i].border_dst[0].y+500) << " " 
          << world->maps[i].comm << "\\001\n";
      }
      f.close();
    }

};


#endif 
