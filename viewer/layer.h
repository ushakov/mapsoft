#ifndef LAYER_H
#define LAYER_H

#include <point.h>
#include <rect.h>
#include <image.h>


// Abstract class Layer

// Layer -- знает, как преобразовать что-то в прямоугольные целочисленные координаты.

class Layer {
public:
    virtual void draw (Image<int> & img, Rect<int> src, Rect<int> dst) = 0;
    virtual Rect<int> range () = 0;

    Layer() {}

    Layer (Layer const & other) { }

    virtual void swap (Layer & other) { }
};



#endif /* LAYER_H */
