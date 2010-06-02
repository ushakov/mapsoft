#ifndef LAYER_RH_H
#define LAYER_RH_H

#include "layer.h"
#include "loaders/image_r.h"
#include "loaders/image_i.h"
#include <math.h>
#include <string>

// Слой для показа растровых картинок. Картинка загружается вся сразу.

class LayerRH : public Layer {

private:
  iImage image;

public:
    LayerRH (const char *file):image(0,0){
	image=image_r::load(file);
    }
    
    // Optimized get_image to return empty image outside of bounds.
    virtual iImage get_image (iRect src){
	if (rect_intersect(image.range(), src).empty()) {
	    return iImage(0,0);
	}
	iImage ret(src.w, src.h, 0);
	draw(src.TLC(), ret);
	return ret;
    }

    virtual void draw (iRect src_rect, iImage & dst_img, Rect<int> dst_rect){
	image_i::load(image, src_rect, dst_img, dst_rect);
    }

    virtual iRect range (){
	return image.range();
    }
};


#endif 
