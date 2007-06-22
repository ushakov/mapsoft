#ifndef LAYER_H
#define LAYER_H

#include "utils/point.h"
#include "utils/rect.h"
#include "utils/image.h"
#include "geo_io/geo_data.h"

#include "layers/action_data.h"

// Abstract class Layer

// Layer -- знает, как отдать некоторую картинку в прямоугольных
// целочисленных координатах. Рассчитан на неоднократное обращение за
// маленькими кусочками


class Layer {
public:
    virtual void draw (Rect<int> src, Image<int> & dst_img, Rect<int> dst) = 0;

    virtual Rect<int> range () = 0;

    Layer() {}
    Layer (Layer const & other) { }
    virtual void swap (Layer & other) { }
};


#endif /* LAYER_H */
