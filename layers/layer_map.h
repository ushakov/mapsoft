#ifndef LAYER_MAP_H
#define LAYER_MAP_H

#include <vector>
#include <sstream>
#include <fstream>
#include <math.h>

#include "layer.h"
#include "../geo_io/geo_data.h"
#include "../utils/mapsoft_convs.h"
#include "../utils/cache.h"
#include "../utils/mapsoft_geo.h"
#include "../loaders/image_r.h"
#include "../loaders/image_i.h"

#include "../geo_io/io.h"


// Слой для показа привязанных карт.

class LayerMap : public Layer {

private:
  const std::vector<g_map> * maps;   // привязки карт
  g_map mymap;                       // информация о привязке layer'a
  std::vector<convs::map2map> m2ms;  // преобразования из каждой карты в mymap
  std::vector<double> scales;        // во сколько раз мы сжимаем карты при загрузке
  std::vector<int>    iscales;       // во сколько раз мы сжимаем карты при загрузке
  Rect<int> map_range;               // габариты карты
  Cache<int, Image<int> > image_cache;    // кэш изображений
  Options O; // для всех карт должны быть одинаковы!

public:

    // построение преобразований, вычисление габаритов карты
    void make_m2ms(){
      if ((maps == NULL)||(maps->size()==0)) return;

      m2ms.clear();
      scales.clear();
      iscales.clear();
      Point<int> brd_min(0xFFFFFF, 0xFFFFFF), brd_max(-0xFFFFFF,-0xFFFFFF);

      for (int i=0; i< maps->size(); i++){
        convs::map2map c((*maps)[i], mymap);
	m2ms.push_back(c);
        
        // определим масштаб
        g_point p1(0,0), p2(1000,0), p3(0,1000);
	c.frw(p1); c.frw(p2); c.frw(p3);
        double sc_x = 1000/sqrt(pow(p2.x-p1.x,2)+pow(p2.y-p1.y,2));
        double sc_y = 1000/sqrt(pow(p3.x-p1.x,2)+pow(p3.y-p1.y,2));

        scales.push_back(sc_x<sc_y ? sc_x:sc_y);
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
    

    // получить/установить привязку layer'a
    g_map get_ref() const {return mymap;}
    void set_ref(const g_map & map){mymap=map; make_m2ms(); image_cache.clear();}

    // установить "естественную привязку"
    void set_ref(){
      if ((maps == NULL)||(maps->size()==0)) return;
      std::vector<g_map>::const_iterator i;

      // Тип проекции возьмем из первой карты. Будем ругаться, если где-то она
      // не совпадет
      // Найдем осевой меридиан -- возьмем среднее по всем точкам привязки
      // и найдем ближайший 6n+3 меридиан
      
      double lon0=0;
      int n=0;
      Proj P((*maps)[0].map_proj);


      for (i=maps->begin(); i!=maps->end(); i++){
	if (P!=i->map_proj)
          std::cerr << "LayerMap warning: different projections in mapset! "
		    << "Using projection of first map (" << P.xml_str() << ")\n";
        for (int j=0; j< i->points.size(); j++){
          lon0+=i->points[j].x; n++;
        }
      }
      lon0/=n;
      lon0 = floor( lon0/6.0 ) * 6 + 3;
      std::ostringstream slon0; slon0 << lon0;
      O["lon0"] = slon0.str();
      O["E0"] = "500000";

      // Определим минимальный масштаб (метров/градусов в точке)
      // поругаемся, если в наборе есть разномасштабные карты
      double mpp_min=1e99;

      for (i=maps->begin(); i!=maps->end(); i++){
	convs::map2pt c(*i, Datum("WGS84"), P, O);
        g_point p1(0,0), p2(1000,0), p3(0,1000);
	c.frw(p1); c.frw(p2); c.frw(p3);

        double mpp_x = sqrt(pow(p2.x-p1.x,2)+pow(p2.y-p1.y,2));
        double mpp_y = sqrt(pow(p3.x-p1.x,2)+pow(p3.y-p1.y,2));

        double mpp = mpp_x < mpp_y ? mpp_x : mpp_y;

	if ((mpp_min < 1e98) && (
	    (mpp_min * 1.1 < mpp)||
	    (mpp_min / 1.1 > mpp)))
          std::cerr << "LayerMap warning: scales of maps in mapset differs more then 10%!\n";
	if (mpp_min>mpp) mpp_min=mpp;
      }
      // точки привязки
      g_point rp1(0,0), rp2(mpp_min, 0), rp3(0, mpp_min);

      convs::pt2pt c2(Datum("WGS84"), P, O, Datum("WGS84"), Proj("lonlat"), O);
      c2.frw(rp1); c2.frw(rp2); c2.frw(rp3);

      mymap.map_proj = P;
      mymap.points.clear();
      mymap.points.push_back(g_refpoint(rp1.x,rp1.y, 0,1000));
      mymap.points.push_back(g_refpoint(rp2.x,rp2.y, 1000,1000));
      mymap.points.push_back(g_refpoint(rp3.x,rp3.y, 0,0));
      // чтоб не пытались определять границы mymap из файла
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

//#ifdef DEBUG_LAYER_MAP
//      std::cerr  << "LayerMap: draw " << src_rect << " -> " 
//		 << dst_rect << " at " << dst_img <<  "\n";
//#endif
	double sc_x = src_rect.w/dst_rect.w;
	double sc_y = src_rect.h/dst_rect.h;

        if ((maps == NULL)||(maps->size()==0)) return;
	for (int i=0; i<maps->size(); i++){

          std::string file = (*maps)[i].file;

          if (!m2ms[i].tst_frw.test_range(src_rect)){  
#ifdef DEBUG_LAYER_MAP
            std::cerr  << "LayerMap: Skipping Image " << file << "\n";
#endif
	        
	    continue;
	  }
          if (!image_cache.contains(i)){

            int scale = int((0.01+scales[i]) * (sc_x<sc_y? sc_x:sc_y));
            if (scale <=0) scale = 1;

//	    scale=1; ///!!!! разбираться, почему scale>1 плохо работает!

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
          << (*maps)[i].comm << "\\001\n";
      }
      f.close();
    }

};


#endif 
