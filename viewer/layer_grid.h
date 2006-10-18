#ifndef LAYER_GRID_H
#define LAYER_GRID_H

#include "layer.h"
#include <math.h>

class LayerGrid : public Layer {
public:
    LayerGrid (double _xstep, double _ystep, int _color)
	: xstep (_xstep), ystep (_ystep), color(_color) { }
    
    virtual void draw (Image<int> & img, Rect<int> src){    

	double sx0 = ((floor(src.x/xstep-1e-20)+1)*xstep-src.x);
	double sy0 = ((floor(src.y/ystep-1e-20)+1)*ystep-src.y);

/*	if (src.x>0) sx0 = xstep - (src.x-1) % xstep - 1;
        else sx0 = (-src.x) % xstep;
	if (src.y>0) sy0 = ystep - (src.y-1) % ystep - 1;
        else sy0 = (-src.y) % ystep;
*/
	// vertical
        for (double x = sx0; x < src.w; x+=xstep){
          for (int y = 0; y < img.h; y++){
	    img.set_na(int((x*img.w)/src.w),y,color);
	  }
	}

	// horizontal
        for (double y = sy0; y < src.h; y+=ystep){
          for (int x = 0; x < img.w; x++){
//	    if ((x-sx0)%xstep != 0) 
	      img.set_na(x,int((y*img.h)/src.h),color);
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
