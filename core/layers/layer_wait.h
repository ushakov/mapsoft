#ifndef LAYER_WAIT_H
#define LAYER_WAIT_H

#include "layer.h"

class LayerWait : public Layer {
public:
    LayerWait (){ }
    
    virtual void draw (iRect src, iImage & img, Rect<int> dst){    

	sleep(1);
    }

    virtual iRect range (){
	return iRect (0,0,0,0);
    }

};


#endif /* LAYER_WAIT_H */
