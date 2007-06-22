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


// ���� ��� ������ ������� google.

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


    // ��������/���������� �������� layer'a
    g_map get_ref() const {return mymap;}

    void set_ref(const g_map & map){mymap=map; cnv = convs::map2map(mymap0,mymap);}
    void set_ref(){set_ref(mymap0);}


    virtual void draw (Rect<int> src_rect, Image<int> & dst_img, Rect<int> dst_rect){

        // ����� ����� �������� �� ����� ���������
	Rect<int> img_rect = cnv.bb_bck(src_rect);

#ifdef DEBUG_LAYER_GOOGLE
        std::cerr  << "LayerGoogle: draw " << src_rect << " -> " 
	           << dst_rect << " at " << dst_img << std::endl;
#endif

        clip_rects_for_image_loader(range(), img_rect, dst_img.range(), dst_rect);
        if (src_rect.empty() || dst_rect.empty()) return;

#ifdef DEBUG_LAYER_GOOGLE
	std::cerr  << "LayerGoogle: inside the map range" <<std::endl;
#endif
	
        // �� ��������� ����� �������� - ������� ��������� ��������
        g_map new_map(mymap0);
        for (g_map::iterator i=new_map.begin(); i!=new_map.end(); i++){
          i->xr-=img_rect.x;
          i->yr-=img_rect.y;
        }
        convs::map2map new_cnv(new_map, mymap);

        // � ����� �������� �� ����� ��������� ��������
	double sc_x = src_rect.w/dst_rect.w;
	double sc_y = src_rect.h/dst_rect.h;
	int sc = int(sc_x<sc_y? sc_x:sc_y);
	if (sc <=0) sc = 1;
        
	Image<int> im0 = google::load(dir, scale, img_rect, sc, do_download);
        new_cnv.image_frw(im0, sc, src_rect, dst_img, dst_rect);
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
