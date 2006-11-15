#ifndef LAYER_RH_H
#define LAYER_RH_H

#include "layer.h"
#include "../loaders/image_r.h"
#include "../loaders/image_i.h"
#include <math.h>
#include <string>

// Слой для показа растровых картинок. Картинка загружается вся сразу.

class LayerRH : public Layer {

private:
  Image<int> image;

public:
    LayerRH (const char *file):image(0,0){
	image=image_r::load(file);
    }
    
    virtual void draw (Rect<int> src_rect, Image<int> & dst_img, Rect<int> dst_rect){
	image_i::load(image, src_rect, dst_img, dst_rect);
    }

    virtual Rect<int> range (){
	return image.range();
    }
};


#endif 
