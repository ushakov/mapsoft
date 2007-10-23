#ifndef LAYER_GOOGLE_H
#define LAYER_GOOGLE_H

#include <vector>
#include <sstream>
#include <fstream>
#include <math.h>

#include "layer_geo.h"
#include "../geo_io/geo_convs.h"
#include "../geo_io/geo_refs.h"
#include "../utils/cache.h"
#include "../loaders/image_google.h"
#include "../loaders/image_i.h"
#include <utils/image_draw.h>

#include "../geo_io/io.h"


// Слой для показа снимков google.

class LayerGoogle : public LayerGeo {
private:
  std::string dir;
  int scale;
  g_map mymap;
  g_map mymap0;
  convs::map2map cnv;
  bool do_download;

public:

    LayerGoogle (const std::string & dir_, const int scale_) : 
      dir(dir_), scale(scale_), mymap0(ref_google(scale)), mymap(ref_google(scale)), cnv(mymap0,mymap), do_download(false){}

    // получить/установить привязку layer'a
    g_map get_ref() const {return mymap;}

    void set_ref(const g_map & map){mymap=map; cnv = convs::map2map(mymap0,mymap);}
    void set_ref(){set_ref(mymap0);}

    virtual void refresh() {}


    virtual void draw (Point<int> origin, Image<int> & image){
        Rect<int> dst_rect = image.range() + origin;
        Rect<int> src_rect = cnv.bb_bck(dst_rect);


#ifdef DEBUG_LAYER_GOOGLE
        std::cerr  << "LayerGoogle: drawing " << dst_rect << std::endl;
        std::cerr  << "LayerGoogle: loading " << src_rect << std::endl;
#endif

        // в каком масштабе мы будем загружать картинку
        double sc_x = src_rect.w/dst_rect.w;
        double sc_y = src_rect.h/dst_rect.h;
        int sc = int(sc_x<sc_y? sc_x:sc_y);
        if (sc <=0) sc = 1;

        // мы загружаем часть картинки - поэтому сбивается привязка
        g_map new_map(mymap0);
        new_map-=src_rect.TLC();
        convs::map2map new_cnv(new_map, mymap);

        Image<int> im0 = google::load(dir, scale, src_rect, sc, do_download);
        new_cnv.image_frw(im0, sc, src_rect, image, dst_rect);
    }


    virtual Rect<int> range (){
	return Rect<int>(0,0,256*(1<<(scale-1)), 256*(1<<(scale-1)));
    }

    void set_downloading (bool downloading)
    {
        do_download = downloading;
    }

    bool get_downloading ()
    {
        return do_download;
    }


};


#endif 
