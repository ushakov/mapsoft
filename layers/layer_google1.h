#ifndef LAYER_GOOGLE1_H
#define LAYER_GOOGLE1_H

#include "layer.h"
#include <image_google.h>

class LayerGoogle1 : public Layer {
public:
    LayerGoogle1 (const std::string & _dir, int _scale)
	: dir (_dir), scale(_scale), do_download(false) { }
    
    virtual void draw (Rect<int> src, Image<int> & img, Rect<int> dst){
	google::load(dir, scale, src, img, dst, do_download);
    }

    virtual Rect<int> range (){
	return Rect<int>(0,0, 
       256*(int)pow(2,scale-1), 
       256*(int)pow(2,scale-1));
    }

    void set_downloading (bool downloading)
    {
	do_download = downloading;
    }

    bool get_downloading ()
    {
	return do_download;
    }
    
private:
    std::string dir;
    int  scale;
    bool do_download;
};


#endif /* LAYER_GOOGLE1_H */
