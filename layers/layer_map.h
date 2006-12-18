#ifndef LAYER_MAP_H
#define LAYER_MAP_H

#include <vector>

#include "layer.h"
#include "../geo_io/geo_data.h"
#include "../utils/mapsoft_convs.h"
#include "../utils/cache.h"
#include "../utils/mapsoft_geo.h"
#include "../loaders/image_r.h"
#include "../loaders/image_i.h"


// Слой для показа привязанных карт.

class LayerMap : public Layer {

private:
  const std::vector<g_map> * maps;   // привязки карт
  g_map mymap;                       // информация о привязке layer'a
  std::vector<convs::map2map> m2ms;  // преобразования из каждой карты в mymap
  std::vector<int> scales;           // во сколько раз мы сжимаем карты при загрузке
  Rect<int> map_range;               // габариты карты
  Cache<std::string, Image<int> > image_cache;    // кэш изображений
  int scale;                         // во сколько раз мы сжимаем карты

public:

    // построение преобразований, вычисление габаритов карты
    void make_m2ms(){
      if ((maps == NULL)||(maps->size()==0)) return;

      m2ms.clear();
      scales.clear();
      Point<int> brd_min(0xFFFFFF, 0xFFFFFF), brd_max(-0xFFFFFF,-0xFFFFFF);

      for (int i=0; i< maps->size(); i++){
        convs::map2map c((*maps)[i], mymap);
	m2ms.push_back(c);
        
        // определим масштаб
        g_point p1(0,0), p2(1000,1000);
std::cerr << ">> "<< p1 << ", " << p2;
	c.frw(p1); c.frw(p2);
std::cerr << " -> "<< p1 << ", " << p2<<"\n";
        double sc_x(fabs(p2.x-p1.x)/1000), sc_y(fabs(p2.y-p1.y)/1000);
	double sc = sc_x<sc_y ? sc_x:sc_y;
        int scale = int(1/sc); if (scale<0) scale=1;
        scales.push_back(scale);

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
    

    // получить/установить привязку layer'a
    g_map get_ref() const {return mymap;}
    void set_ref(const g_map & map){mymap=map; make_m2ms(); image_cache.clear();}

    // установить "естественную привязку"
    void set_ref(){
      if ((maps == NULL)||(maps->size()==0)) return;
      std::vector<g_map>::const_iterator i;

      // Тип проекции возьмем из первой карты. Будем ругаться, если где-то она
      // не совпадет
      // Определим минимальный масштаб (метров/градусов в точке)
      // поругаемся, если в наборе есть разномасштабные карты
      // Найдем осевой меридиан -- возьмем среднее по всем точкам привязки
      // и найдем ближайший 6n+3 меридиан
      
      double mpp_min=1e99;
      double lon0=0;
      int n=0;
      Proj P((*maps)[0].map_proj);


      for (i=maps->begin(); i!=maps->end(); i++){
	convs::map2pt c(*i, Datum("WGS84"), P, Options());
        g_point p1(0,0), p2(1000,1000);
	c.frw(p1); c.frw(p2);

        double mpp_x(fabs(p2.x-p1.x)), mpp_y(fabs(p2.y-p1.y));
        double mpp = mpp_x < mpp_y ? mpp_x : mpp_y;

std::cerr << ">> mpp_x: " << mpp_x << " mpp_y: " << mpp_y << "\n";
//std::cerr << ">> p1: " << int(p1.x) << " " << int(p1.y) << " p2: " << int(p2.x) << " " << int(p2.y) << "\n";

	if ((mpp_min < 1e98) && (
	    (mpp_min * 1.1 < mpp)||
	    (mpp_min / 1.1 > mpp)))
          std::cerr << "LayerMap warning: scales of maps in mapset differs more then 10%!\n";
	if (mpp_min>mpp) mpp_min=mpp;

        for (int j=0; j< i->points.size(); j++){
          lon0+=i->points[j].x; n++;
        }
	if (P!=i->map_proj)
          std::cerr << "LayerMap warning: different projections in mapset! "
		    << "Using projection of first map (" << P.xml_str() << ")\n";
      }
      lon0/=n;
      lon0 = floor( lon0/6.0 ) * 6 + 3;
      
      mymap.map_proj = P;
      mymap.points.clear();
      mymap.points.push_back(g_refpoint(lon0,0, 0,0));
      mymap.points.push_back(g_refpoint(lon0+mpp_min,0, 1000,0));
      mymap.points.push_back(g_refpoint(lon0,mpp_min, 0,1000));
      // чтоб не пытались определять границы из файла
      g_point bp(0,0);
      mymap.border.push_back(bp);
      mymap.border.push_back(bp);
      mymap.border.push_back(bp);

#ifdef DEBUG_LAYER_MAP
      std::cerr << "LayerMap: Setting map reference. Proj: " << P.xml_str() 
		<< " Lon0: " << lon0 
		<< " Scale: " << mpp_min/1000 << " meters/degrees per pixel\n";
#endif
      make_m2ms();
      image_cache.clear();
    }

    LayerMap (const std::vector<g_map> * _maps) : maps(_maps), image_cache(4) { set_ref(); }
    
    virtual void draw (Rect<int> src_rect, Image<int> & dst_img, Rect<int> dst_rect){
        if ((maps == NULL)||(maps->size()==0)) return;
	for (int i=0; i<maps->size(); i++){
          if (!image_cache.contains((*maps)[i].file)){
#ifdef DEBUG_LAYER_MAP
      std::cerr  << "LayerMap: Loading Image " << (*maps)[i].file
		 << " at scale " << scales[i] <<  "\n";
#endif

            image_cache.add((*maps)[i].file, image_r::load((*maps)[i].file.c_str(), scales[i]));
          }
	  Image<int> im = image_cache.get((*maps)[i].file);
          m2ms[i].image_frw(im, im.range(), scales[i], dst_img, dst_rect);
        }
    }

    virtual Rect<int> range (){
	return map_range;
    }
};


#endif 
