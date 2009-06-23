#ifndef LAYER_CGRID_H
#define LAYER_CGRID_H

#include "layer.h"

// Constant grid layer. Draws grid, knows nothing about scales

class LayerCGrid : public Layer {
public:
    LayerCGrid (int _step=100, int _color=0xFF808080)
      : step (_step), color(_color) { }

    iRect range(){return Rect<int>(0,0,0,0);}
    void refresh(){}

    iImage get_image (iRect src){

std::cerr << src.x/256 << " " << src.y/256 <<  "\n";

      iPoint sh;
      sh.x = src.x - (src.x/step)*step;
      sh.y = src.y - (src.y/step)*step;

      if (src.x<0) sh.x+=step+1;
      if (src.y<0) sh.y+=step+1;

      iImage ret(src.w, src.h, 0);
      for (int x=step-sh.x; x<src.w; x+=step){
         for (int y=0; y<src.h; y++) ret.set_na(x,y,color);
      }
      for (int y=step-sh.y; y<src.h; y+=step){
         for (int x=0; x<src.w; x++) ret.set_na(x,y,color);
      }
      return ret;
    }

    int color;
    int step;
};


#endif /* LAYER_CGRID_H */
