#ifndef LAYER_GRID_H
#define LAYER_GRID_H

#include "layer.h"
#include <math.h>

class LayerGrid : public Layer {
public:
    LayerGrid (double _xstep, double _ystep, int _color)
	: xstep (_xstep), ystep (_ystep), color(_color) { }
    
    virtual void draw (Image<int> & img, Rect<int> src, Rect<int> dst){    

        clip_rects_for_image_loader(src, src, img.range(), dst);

std::cerr << src << " -> " << dst << "\nimage: "<< img << "\n";

	double sx0 = ((floor(src.x/xstep-1e-20)+1)*xstep-src.x);
	double sy0 = ((floor(src.y/ystep-1e-20)+1)*ystep-src.y);

	// vertical
        for (double x = sx0; x < src.w; x+=xstep){
          for (int y = 0; y < dst.h; y++){
	    img.set_na(int((x*dst.w)/src.w+dst.x), y+dst.y, color);
	  }
	}

	// horizontal
        for (double y = sy0; y < src.h; y+=ystep){
          for (int x = 0; x < dst.w; x++){
//	    if ((x-sx0)%xstep != 0) 
	      img.set_na(x+dst.x, int((y*dst.h)/src.h+dst.y), color);
	  } 
	}
    }

    virtual Rect<int> range (){
	return Rect<int> (0,0,0,0);
    }

    void set_steps (int _xstep, int _ystep)
    {
	xstep = _xstep;
	ystep = _ystep;
    }

    void set_color (int _color)
    {
	color = _color;
    }

private:
    int color;
    double xstep, ystep;
};


#endif /* LAYER_GRID_H */
