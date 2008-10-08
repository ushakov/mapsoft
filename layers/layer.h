#ifndef LAYER_H
#define LAYER_H

#include "../lib2d/image.h"
#include "../libgeo/geo_data.h"

//#include "action_data.h"

/// Растровый слой -- абстрактный класс.
/// Знает, как отдать некоторую растровую картинку в прямоугольных
/// целочисленных координатах. Рассчитан на неоднократное обращение за
/// маленькими кусочками

class Layer:
  public boost::multiplicative<Layer,double>,
  public boost::additive<Layer, g_point>
 {
public:
//    virtual void draw (Rect<int> src, Image<int> & dst_img, Rect<int> dst) = 0;
    virtual void draw(const Point<int> origin, Image<int> & image) = 0;

    virtual Image<int> get_image (Rect<int> src){
	Image<int> ret(src.w, src.h, 0);
	draw(src.TLC(), ret);
	return ret;
    }

    virtual Rect<int> range () = 0;
    virtual void refresh() = 0;

    virtual Layer & operator/= (double k) = 0;
    virtual Layer & operator*= (double k) = 0;
    virtual Layer & operator-= (g_point k) = 0;
    virtual Layer & operator+= (g_point k) = 0;

    Layer() {}
    Layer (Layer const & other) { }
    virtual void swap (Layer & other) { }
};


#endif /* LAYER_H */
