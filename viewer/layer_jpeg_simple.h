#ifndef LAYER_JPEG_SIMPLE_H
#define LAYER_JPEG_SIMPLE_H

#include "layer.h"
#include <image_jpeg.h>

class LayerJpegSimple : public Layer {
public:
    LayerJpegSimple (char * _file)
	: file (_file) { }
    
    virtual void draw (Image<int> & img, Rect<int> src, Rect<int> dst){    
	image_jpeg::load_to_image(file, src, img, dst);
    }

    virtual Rect<int> range (){
	return Rect<int>(Point<int>(0,0), image_jpeg::size(file));
    }


private:
    char *file;
};


#endif /* LAYER_JPEG_SIMPLE_H */
