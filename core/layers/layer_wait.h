#ifndef LAYER_WAIT_H
#define LAYER_WAIT_H

#include "layer.h"

class LayerWait : public Layer {
public:
    LayerWait (){ }
    
    virtual void draw (Rect<int> src, Image<int> & img, Rect<int> dst){    

	sleep(1);
    }

    virtual Rect<int> range (){
	return Rect<int> (0,0,0,0);
    }

};


#endif /* LAYER_WAIT_H */
