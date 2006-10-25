#ifndef LAYER_GOOGLE1_H
#define LAYER_GOOGLE1_H

#include "layer.h"
#include <image_google.h>

class LayerGoogle1 : public Layer {
public:
    LayerGoogle1 (const std::string & _dir, int _scale)
	: dir (_dir), scale(_scale) { }
    
    virtual void draw (Image<int> & img, Rect<int> src){    
	google::load_to_image(dir, scale, src, img, img.range());
    }

    virtual Rect<int> range (){
	return Rect<int>(0,0, 
       256*(int)pow(2,scale-1), 
       256*(int)pow(2,scale-1));
    }

private:
    std::string dir;
    int  scale;
};


#endif /* LAYER_GOOGLE1_H */
