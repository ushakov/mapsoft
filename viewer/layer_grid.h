#ifndef LAYER_GRID_H
#define LAYER_GRID_H

#include "layer.h"

class LayerGrid : public Layer {
public:
    LayerGrid (int _xstep, int _ystep, int _color)
	: xstep (_xstep), ystep (_ystep), color(_color) { }
    
    virtual void draw (Image<int> & img, Rect<int> src){    

	int sx0,sy0;
	if (src.x>0) sx0 = xstep - (src.x-1) % xstep - 1;
        else sx0 = (-src.x) % xstep;
	if (src.y>0) sy0 = ystep - (src.y-1) % ystep - 1;
        else sy0 = (-src.y) % ystep;

	// vertical
        for (int x = sx0; x < src.w; x+=xstep){
          std::cerr << "greed: " << x << "\n";
          for (int y = 0; y < img.h; y++){
	    img.set_na((x*img.w)/src.w,y,color);
	  }
	}

	// horizontal
        for (int y = sy0; y < src.h; y+=ystep){
          for (int x = 0; x < img.w; x++){
	    if ((x-sx0)%xstep != 0) 
	      img.set_na(x,(y*img.h)/src.h,color);
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
    int xstep, ystep, color;
};


#endif /* LAYER_GRID_H */
