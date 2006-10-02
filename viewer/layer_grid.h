#ifndef LAYER_GRID_H
#define LAYER_GRID_H

#include "layer.h"

class LayerGrid : public Layer {
public:
    LayerGrid (int _xstep, int _ystep, int _color)
	: xstep (_xstep), ystep (_ystep), color(_color) { }
    
    virtual void draw (Image<int> & img, Rect<int> src, Rect<int> dst){    

	std::cerr << "LayerGrid: "<< src << " -> " << dst << "\n";
	clip_rect_to_rect(dst, img.range());

	int dx = (xstep*dst.w)/src.w;
	int dy = (ystep*dst.h)/src.h;

	int sx0,sy0;
	if (src.x>0) sx0 = xstep - src.x % xstep - 1;
        else sx0 = (-src.x) % xstep;
	if (src.y>0) sy0 = ystep - src.y % ystep - 1;
        else sy0 = (-src.y) % ystep;

	int x0 = (sx0*dst.w)/src.w;
	int y0 = (sy0*dst.h)/src.h;	

	std::cerr << "x0: "<< x0 
	          << "y0: "<< y0 
	          << "dx: "<< dx
	          << "dy: "<< dy 
                  << "\n";

	// vertical
        for (int x = x0; x < dst.x+dst.w; x+=dx){
          for (int y = dst.y; y < dst.y+dst.h; y++){
	    img.set_na(x,y,color);
	  }
	}

	// horizontal
        for (int y = y0; y < dst.y+dst.h; y+=dy){
          for (int x = dst.x; x < dst.x+dst.w; x++){
	    if ((x-x0)%xstep != 0) img.set_na(x,y,color);
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
